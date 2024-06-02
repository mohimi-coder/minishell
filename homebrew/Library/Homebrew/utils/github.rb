# typed: true
# frozen_string_literal: true

require "uri"
require "utils/github/actions"
require "utils/github/api"

require "system_command"

# A module that interfaces with GitHub, code like PAT scopes, credential handling and API errors.
#
# @api internal
module GitHub
  extend SystemCommand::Mixin

  def self.check_runs(repo: nil, commit: nil, pull_request: nil)
    if pull_request
      repo = pull_request.fetch("base").fetch("repo").fetch("full_name")
      commit = pull_request.fetch("head").fetch("sha")
    end

    API.open_rest(url_to("repos", repo, "commits", commit, "check-runs"))
  end

  def self.create_check_run(repo:, data:)
    API.open_rest(url_to("repos", repo, "check-runs"), data:)
  end

  def self.issues(repo:, **filters)
    uri = url_to("repos", repo, "issues")
    uri.query = URI.encode_www_form(filters)
    API.open_rest(uri)
  end

  def self.search_issues(query, **qualifiers)
    search_results_items("issues", query, **qualifiers)
  end

  def self.count_issues(query, **qualifiers)
    search_results_count("issues", query, **qualifiers)
  end

  def self.create_gist(files, description, private:)
    url = "#{API_URL}/gists"
    data = { "public" => !private, "files" => files, "description" => description }
    API.open_rest(url, data:, scopes: CREATE_GIST_SCOPES)["html_url"]
  end

  def self.create_issue(repo, title, body)
    url = "#{API_URL}/repos/#{repo}/issues"
    data = { "title" => title, "body" => body }
    API.open_rest(url, data:, scopes: CREATE_ISSUE_FORK_OR_PR_SCOPES)["html_url"]
  end

  def self.repository(user, repo)
    API.open_rest(url_to("repos", user, repo))
  end

  def self.issues_for_formula(name, tap: CoreTap.instance, tap_remote_repo: tap&.full_name, state: nil, type: nil)
    return [] unless tap_remote_repo

    search_issues(name, repo: tap_remote_repo, state:, type:, in: "title")
  end

  def self.user
    @user ||= API.open_rest("#{API_URL}/user")
  end

  def self.permission(repo, user)
    API.open_rest("#{API_URL}/repos/#{repo}/collaborators/#{user}/permission")
  end

  def self.write_access?(repo, user = nil)
    user ||= self.user["login"]
    ["admin", "write"].include?(permission(repo, user)["permission"])
  end

  def self.branch_exists?(user, repo, branch)
    API.open_rest("#{API_URL}/repos/#{user}/#{repo}/branches/#{branch}")
    true
  rescue API::HTTPNotFoundError
    false
  end

  def self.pull_requests(repo, **options)
    url = "#{API_URL}/repos/#{repo}/pulls?#{URI.encode_www_form(options)}"
    API.open_rest(url)
  end

  def self.merge_pull_request(repo, number:, sha:, merge_method:, commit_message: nil)
    url = "#{API_URL}/repos/#{repo}/pulls/#{number}/merge"
    data = { sha:, merge_method: }
    data[:commit_message] = commit_message if commit_message
    API.open_rest(url, data:, request_method: :PUT, scopes: CREATE_ISSUE_FORK_OR_PR_SCOPES)
  end

  def self.print_pull_requests_matching(query, only = nil)
    open_or_closed_prs = search_issues(query, is: only, type: "pr", user: "Homebrew")

    open_prs, closed_prs = open_or_closed_prs.partition { |pr| pr["state"] == "open" }
                                             .map { |prs| prs.map { |pr| "#{pr["title"]} (#{pr["html_url"]})" } }

    if open_prs.present?
      ohai "Open pull requests"
      open_prs.each { |pr| puts pr }
    end

    if closed_prs.present?
      puts if open_prs.present?

      ohai "Closed pull requests"
      closed_prs.take(20).each { |pr| puts pr }

      puts "..." if closed_prs.count > 20
    end

    puts "No pull requests found for #{query.inspect}" if open_prs.blank? && closed_prs.blank?
  end

  def self.create_fork(repo, org: nil)
    url = "#{API_URL}/repos/#{repo}/forks"
    data = {}
    data[:organization] = org if org
    scopes = CREATE_ISSUE_FORK_OR_PR_SCOPES
    API.open_rest(url, data:, scopes:)
  end

  def self.fork_exists?(repo, org: nil)
    _, reponame = repo.split("/")

    username = org || API.open_rest(url_to("user")) { |json| json["login"] }
    json = API.open_rest(url_to("repos", username, reponame))

    return false if json["message"] == "Not Found"

    true
  end

  def self.create_pull_request(repo, title, head, base, body)
    url = "#{API_URL}/repos/#{repo}/pulls"
    data = { title:, head:, base:, body:, maintainer_can_modify: true }
    scopes = CREATE_ISSUE_FORK_OR_PR_SCOPES
    API.open_rest(url, data:, scopes:)
  end

  def self.private_repo?(full_name)
    uri = url_to "repos", full_name
    API.open_rest(uri) { |json| json["private"] }
  end

  def self.search_query_string(*main_params, **qualifiers)
    params = main_params

    from = qualifiers.fetch(:from, nil)
    to = qualifiers.fetch(:to, nil)

    params << if from && to
      "created:#{from}..#{to}"
    elsif from
      "created:>=#{from}"
    elsif to
      "created:<=#{to}"
    end

    params += qualifiers.except(:args, :from, :to).flat_map do |key, value|
      Array(value).map { |v| "#{key.to_s.tr("_", "-")}:#{v}" }
    end

    "q=#{URI.encode_www_form_component(params.compact.join(" "))}&per_page=100"
  end

  def self.url_to(*subroutes)
    URI.parse([API_URL, *subroutes].join("/"))
  end

  def self.search(entity, *queries, **qualifiers)
    uri = url_to "search", entity
    uri.query = search_query_string(*queries, **qualifiers)
    API.open_rest(uri)
  end

  def self.search_results_items(entity, *queries, **qualifiers)
    json = search(entity, *queries, **qualifiers)
    json.fetch("items", [])
  end

  def self.search_results_count(entity, *queries, **qualifiers)
    json = search(entity, *queries, **qualifiers)
    json.fetch("total_count", 0)
  end

  def self.approved_reviews(user, repo, pull_request, commit: nil)
    query = <<~EOS
      { repository(name: "#{repo}", owner: "#{user}") {
          pullRequest(number: #{pull_request}) {
            reviews(states: APPROVED, first: 100) {
              nodes {
                author {
                  ... on User { email login name databaseId }
                  ... on Organization { email login name databaseId }
                }
                authorAssociation
                commit { oid }
              }
            }
          }
        }
      }
    EOS

    result = API.open_graphql(query, scopes: ["user:email"])
    reviews = result["repository"]["pullRequest"]["reviews"]["nodes"]

    valid_associations = %w[MEMBER OWNER]
    reviews.filter_map do |r|
      next if commit.present? && commit != r["commit"]["oid"]
      next unless valid_associations.include? r["authorAssociation"]

      email = r["author"]["email"].presence ||
              "#{r["author"]["databaseId"]}+#{r["author"]["login"]}@users.noreply.github.com"

      name = r["author"]["name"].presence ||
             r["author"]["login"]

      {
        "email" => email,
        "name"  => name,
        "login" => r["author"]["login"],
      }
    end
  end

  def self.dispatch_event(user, repo, event, **payload)
    url = "#{API_URL}/repos/#{user}/#{repo}/dispatches"
    API.open_rest(url, data:           { event_type: event, client_payload: payload },
                       request_method: :POST,
                       scopes:         CREATE_ISSUE_FORK_OR_PR_SCOPES)
  end

  def self.workflow_dispatch_event(user, repo, workflow, ref, **inputs)
    url = "#{API_URL}/repos/#{user}/#{repo}/actions/workflows/#{workflow}/dispatches"
    API.open_rest(url, data:           { ref:, inputs: },
                       request_method: :POST,
                       scopes:         CREATE_ISSUE_FORK_OR_PR_SCOPES)
  end

  def self.get_release(user, repo, tag)
    url = "#{API_URL}/repos/#{user}/#{repo}/releases/tags/#{tag}"
    API.open_rest(url, request_method: :GET)
  end

  def self.get_latest_release(user, repo)
    url = "#{API_URL}/repos/#{user}/#{repo}/releases/latest"
    API.open_rest(url, request_method: :GET)
  end

  def self.generate_release_notes(user, repo, tag, previous_tag: nil)
    url = "#{API_URL}/repos/#{user}/#{repo}/releases/generate-notes"
    data = { tag_name: tag }
    data[:previous_tag_name] = previous_tag if previous_tag.present?
    API.open_rest(url, data:, request_method: :POST, scopes: CREATE_ISSUE_FORK_OR_PR_SCOPES)
  end

  def self.create_or_update_release(user, repo, tag, id: nil, name: nil, body: nil, draft: false)
    url = "#{API_URL}/repos/#{user}/#{repo}/releases"
    method = if id
      url += "/#{id}"
      :PATCH
    else
      :POST
    end
    data = {
      tag_name: tag,
      name:     name || tag,
      draft:,
    }
    data[:body] = body if body.present?
    API.open_rest(url, data:, request_method: method, scopes: CREATE_ISSUE_FORK_OR_PR_SCOPES)
  end

  def self.upload_release_asset(user, repo, id, local_file: nil, remote_file: nil)
    url = "https://uploads.github.com/repos/#{user}/#{repo}/releases/#{id}/assets"
    url += "?name=#{remote_file}" if remote_file
    API.open_rest(url, data_binary_path: local_file, request_method: :POST, scopes: CREATE_ISSUE_FORK_OR_PR_SCOPES)
  end

  def self.get_workflow_run(user, repo, pull_request, workflow_id: "tests.yml", artifact_pattern: "bottles{,_*}")
    scopes = CREATE_ISSUE_FORK_OR_PR_SCOPES

    # GraphQL unfortunately has no way to get the workflow yml name, so we need an extra REST call.
    workflow_api_url = "#{API_URL}/repos/#{user}/#{repo}/actions/workflows/#{workflow_id}"
    workflow_payload = API.open_rest(workflow_api_url, scopes:)
    workflow_id_num = workflow_payload["id"]

    query = <<~EOS
      query ($user: String!, $repo: String!, $pr: Int!) {
        repository(owner: $user, name: $repo) {
          pullRequest(number: $pr) {
            commits(last: 1) {
              nodes {
                commit {
                  checkSuites(first: 100) {
                    nodes {
                      status,
                      workflowRun {
                        databaseId,
                        url,
                        workflow {
                          databaseId
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    EOS
    variables = {
      user:,
      repo:,
      pr:   pull_request.to_i,
    }
    result = API.open_graphql(query, variables:, scopes:)

    commit_node = result["repository"]["pullRequest"]["commits"]["nodes"].first
    check_suite = if commit_node.present?
      commit_node["commit"]["checkSuites"]["nodes"].select do |suite|
        suite.dig("workflowRun", "workflow", "databaseId") == workflow_id_num
      end
    else
      []
    end

    [check_suite, user, repo, pull_request, workflow_id, scopes, artifact_pattern]
  end

  def self.get_artifact_urls(workflow_array)
    check_suite, user, repo, pr, workflow_id, scopes, artifact_pattern = *workflow_array
    if check_suite.empty?
      raise API::Error, <<~EOS
        No matching check suite found for these criteria!
          Pull request: #{pr}
          Workflow:     #{workflow_id}
      EOS
    end

    status = check_suite.last["status"].sub("_", " ").downcase
    if status != "completed"
      raise API::Error, <<~EOS
        The newest workflow run for ##{pr} is still #{status}!
          #{Formatter.url check_suite.last["workflowRun"]["url"]}
      EOS
    end

    run_id = check_suite.last["workflowRun"]["databaseId"]
    artifacts = []
    per_page = 50
    API.paginate_rest("#{API_URL}/repos/#{user}/#{repo}/actions/runs/#{run_id}/artifacts",
                      per_page:, scopes:) do |result|
      result = result["artifacts"]
      artifacts.concat(result)
      break if result.length < per_page
    end

    matching_artifacts =
      artifacts
      .group_by { |art| art["name"] }
      .select { |name| File.fnmatch?(artifact_pattern, name, File::FNM_EXTGLOB) }
      .map { |_, arts| arts.last }

    if matching_artifacts.empty?
      raise API::Error, <<~EOS
        No artifacts with the pattern `#{artifact_pattern}` were found!
          #{Formatter.url check_suite.last["workflowRun"]["url"]}
      EOS
    end

    matching_artifacts.map { |art| art["archive_download_url"] }
  end

  def self.public_member_usernames(org, per_page: 100)
    url = "#{API_URL}/orgs/#{org}/public_members"
    members = []

    API.paginate_rest(url, per_page:) do |result|
      result = result.map { |member| member["login"] }
      members.concat(result)

      return members if result.length < per_page
    end
  end

  def self.members_by_team(org, team)
    query = <<~EOS
        { organization(login: "#{org}") {
          teams(first: 100) {
            nodes {
              ... on Team { name }
            }
          }
          team(slug: "#{team}") {
            members(first: 100) {
              nodes {
                ... on User { login name }
              }
            }
          }
        }
      }
    EOS
    result = API.open_graphql(query, scopes: ["read:org", "user"])

    if result["organization"]["teams"]["nodes"].blank?
      raise API::Error,
            "Your token needs the 'read:org' scope to access this API"
    end
    raise API::Error, "The team #{org}/#{team} does not exist" if result["organization"]["team"].blank?

    result["organization"]["team"]["members"]["nodes"].to_h { |member| [member["login"], member["name"]] }
  end

  sig {
    params(user: String)
      .returns(
        T::Array[{
          closest_tier_monthly_amount: Integer,
          login:                       String,
          monthly_amount:              Integer,
          name:                        String,
        }],
      )
  }
  def self.sponsorships(user)
    has_next_page = T.let(true, T::Boolean)
    after = ""
    sponsorships = T.let([], T::Array[Hash])
    errors = T.let([], T::Array[Hash])
    while has_next_page
      query = <<~EOS
          { organization(login: "#{user}") {
            sponsorshipsAsMaintainer(first: 100 #{after}) {
              pageInfo {
                startCursor
                hasNextPage
                endCursor
              }
              totalCount
              nodes {
                tier {
                  monthlyPriceInDollars
                  closestLesserValueTier {
                    monthlyPriceInDollars
                  }
                }
                sponsorEntity {
                  __typename
                  ... on Organization { login name }
                  ... on User { login name }
                }
              }
            }
          }
        }
      EOS
      # Some organisations do not permit themselves to be queried through the
      # API like this and raise an error so handle these errors later.
      # This has been reported to GitHub.
      result = API.open_graphql(query, scopes: ["user"], raise_errors: false)
      errors += result["errors"] if result["errors"].present?

      current_sponsorships = result["data"]["organization"]["sponsorshipsAsMaintainer"]

      # The organisations mentioned above will show up as nil nodes.
      if (nodes = current_sponsorships["nodes"].compact.presence)
        sponsorships += nodes
      end

      if (page_info = current_sponsorships["pageInfo"].presence) &&
         page_info["hasNextPage"].presence
        after = %Q(, after: "#{page_info["endCursor"]}")
      else
        has_next_page = false
      end
    end

    # Only raise errors if we didn't get any sponsorships.
    if sponsorships.blank? && errors.present?
      raise API::Error, errors.map { |e| "#{e["type"]}: #{e["message"]}" }.join("\n")
    end

    sponsorships.map do |sponsorship|
      sponsor = sponsorship["sponsorEntity"]
      tier = sponsorship["tier"].presence || {}
      monthly_amount = tier["monthlyPriceInDollars"].presence || 0
      closest_tier = tier["closestLesserValueTier"].presence || {}
      closest_tier_monthly_amount = closest_tier["monthlyPriceInDollars"].presence || 0

      {
        name:                        sponsor["name"].presence || sponsor["login"],
        login:                       sponsor["login"],
        monthly_amount:,
        closest_tier_monthly_amount:,
      }
    end
  end

  def self.get_repo_license(user, repo, ref: nil)
    url = "#{API_URL}/repos/#{user}/#{repo}/license"
    url += "?ref=#{ref}" if ref.present?
    response = API.open_rest(url)
    return unless response.key?("license")

    response["license"]["spdx_id"]
  rescue API::HTTPNotFoundError
    nil
  rescue API::AuthenticationFailedError => e
    raise unless e.message.match?(API::GITHUB_IP_ALLOWLIST_ERROR)
  end

  def self.pull_request_title_regex(name, version = nil)
    return /(^|\s)#{Regexp.quote(name)}(:|,|\s|$)/i if version.blank?

    /(^|\s)#{Regexp.quote(name)}(:|,|\s)(.*\s)?#{Regexp.quote(version)}(:|,|\s|$)/i
  end

  sig {
    params(name: String, tap_remote_repo: String, state: T.nilable(String), version: T.nilable(String))
      .returns(T::Array[T::Hash[String, T.untyped]])
  }
  def self.fetch_pull_requests(name, tap_remote_repo, state: nil, version: nil)
    return [] if Homebrew::EnvConfig.no_github_api?

    regex = pull_request_title_regex(name, version)
    query = "is:pr #{name} #{version}".strip

    # Unauthenticated users cannot use GraphQL so use search REST API instead.
    # Limit for this is 30/minute so is usually OK unless you're spamming bump PRs (e.g. CI).
    if API.credentials_type == :none
      return issues_for_formula(query, tap_remote_repo:, state:).select do |pr|
        pr["html_url"].include?("/pull/") && regex.match?(pr["title"])
      end
    elsif state == "open" && ENV["GITHUB_REPOSITORY_OWNER"] == "Homebrew"
      # Try use PR API, which might be cheaper on rate limits in some cases.
      # The rate limit of the search API under GraphQL is unclear as it
      # costs the same as any other query according to /rate_limit.
      # The PR API is also not very scalable so limit to Homebrew CI.
      return fetch_open_pull_requests(name, tap_remote_repo, version:)
    end

    query += " repo:#{tap_remote_repo} in:title"
    query += " state:#{state}" if state.present?
    graphql_query = <<~EOS
      query($query: String!, $after: String) {
        search(query: $query, type: ISSUE, first: 100, after: $after) {
          nodes {
            ... on PullRequest {
              number
              title
              url
              state
            }
          }
          pageInfo {
            hasNextPage
            endCursor
          }
        }
      }
    EOS
    variables = { query: }

    pull_requests = []
    API.paginate_graphql(graphql_query, variables:) do |result|
      data = result["search"]
      pull_requests.concat(data["nodes"].select { |pr| regex.match?(pr["title"]) })
      data["pageInfo"]
    end
    pull_requests.map! do |pr|
      pr.merge({
        "html_url" => pr.delete("url"),
        "state"    => pr.fetch("state").downcase,
      })
    end
  rescue API::RateLimitExceededError => e
    opoo e.message
    pull_requests || []
  end

  def self.fetch_open_pull_requests(name, tap_remote_repo, version: nil)
    return [] if tap_remote_repo.blank?

    # Bust the cache every three minutes.
    cache_expiry = 3 * 60
    cache_epoch = Time.now - (Time.now.to_i % cache_expiry)
    cache_key = "#{tap_remote_repo}_#{cache_epoch.to_i}"

    @open_pull_requests ||= {}
    @open_pull_requests[cache_key] ||= begin
      query = <<~EOS
        query($owner: String!, $repo: String!, $states: [PullRequestState!], $after: String) {
          repository(owner: $owner, name: $repo) {
            pullRequests(states: $states, first: 100, after: $after) {
              nodes {
                number
                title
                url
              }
              pageInfo {
                hasNextPage
                endCursor
              }
            }
          }
        }
      EOS
      owner, repo = tap_remote_repo.split("/")
      variables = { owner:, repo:, states: ["OPEN"] }

      pull_requests = []
      API.paginate_graphql(query, variables:) do |result|
        data = result.dig("repository", "pullRequests")
        pull_requests.concat(data["nodes"])
        data["pageInfo"]
      end
      pull_requests
    end

    regex = pull_request_title_regex(name, version)
    @open_pull_requests[cache_key].select { |pr| regex.match?(pr["title"]) }
                                  .map { |pr| pr.merge("html_url" => pr.delete("url")) }
  rescue API::RateLimitExceededError => e
    opoo e.message
    pull_requests || []
  end

  def self.check_for_duplicate_pull_requests(name, tap_remote_repo, state:, file:, quiet:, version: nil)
    pull_requests = fetch_pull_requests(name, tap_remote_repo, state:, version:)

    pull_requests.select! do |pr|
      get_pull_request_changed_files(
        tap_remote_repo, pr["number"]
      ).any? { |f| f["filename"] == file }
    end
    return if pull_requests.blank?

    duplicates_message = <<~EOS
      These #{state} pull requests may be duplicates:
      #{pull_requests.map { |pr| "#{pr["title"]} #{pr["html_url"]}" }.join("\n")}
    EOS
    error_message = <<~EOS
      Duplicate PRs should not be opened.
      Manually open these PRs if you are sure that they are not duplicates.
    EOS

    if quiet
      odie error_message
    else
      odie <<~EOS
        #{duplicates_message.chomp}
        #{error_message}
      EOS
    end
  end

  def self.get_pull_request_changed_files(tap_remote_repo, pull_request)
    API.open_rest(url_to("repos", tap_remote_repo, "pulls", pull_request, "files"))
  end

  private_class_method def self.add_auth_token_to_url!(url)
    if API.credentials_type == :env_token
      url.sub!(%r{^https://github\.com/}, "https://#{API.credentials}@github.com/")
    end
    url
  end

  def self.forked_repo_info!(tap_remote_repo, org: nil)
    response = create_fork(tap_remote_repo, org:)
    # GitHub API responds immediately but fork takes a few seconds to be ready.
    sleep 1 until fork_exists?(tap_remote_repo, org:)
    remote_url = if system("git", "config", "--local", "--get-regexp", "remote..*.url", "git@github.com:.*")
      response.fetch("ssh_url")
    else
      add_auth_token_to_url!(response.fetch("clone_url"))
    end
    username = response.fetch("owner").fetch("login")
    [remote_url, username]
  end

  def self.create_bump_pr(info, args:)
    tap = info[:tap]
    sourcefile_path = info[:sourcefile_path]
    old_contents = info[:old_contents]
    additional_files = info[:additional_files] || []
    remote = info[:remote] || "origin"
    remote_branch = info[:remote_branch] || tap.git_repo.origin_branch_name
    branch = info[:branch_name]
    commit_message = info[:commit_message]
    previous_branch = info[:previous_branch] || "-"
    tap_remote_repo = info[:tap_remote_repo] || tap.full_name
    pr_message = info[:pr_message]

    sourcefile_path.parent.cd do
      git_dir = Utils.popen_read("git", "rev-parse", "--git-dir").chomp
      shallow = !git_dir.empty? && File.exist?("#{git_dir}/shallow")
      changed_files = [sourcefile_path]
      changed_files += additional_files if additional_files.present?

      if args.dry_run? || (args.write_only? && !args.commit?)
        remote_url = if args.no_fork?
          Utils.popen_read("git", "remote", "get-url", "--push", "origin").chomp
        else
          fork_message = "try to fork repository with GitHub API" \
                         "#{" into `#{args.fork_org}` organization" if args.fork_org}"
          ohai fork_message
          "FORK_URL"
        end
        ohai "git fetch --unshallow origin" if shallow
        ohai "git add #{changed_files.join(" ")}"
        ohai "git checkout --no-track -b #{branch} #{remote}/#{remote_branch}"
        ohai "git commit --no-edit --verbose --message='#{commit_message}' " \
             "-- #{changed_files.join(" ")}"
        ohai "git push --set-upstream #{remote_url} #{branch}:#{branch}"
        ohai "git checkout --quiet #{previous_branch}"
        ohai "create pull request with GitHub API (base branch: #{remote_branch})"
      else

        unless args.commit?
          if args.no_fork?
            remote_url = Utils.popen_read("git", "remote", "get-url", "--push", "origin").chomp
            add_auth_token_to_url!(remote_url)
            username = tap.user
          else
            begin
              remote_url, username = forked_repo_info!(tap_remote_repo, org: args.fork_org)
            rescue *API::ERRORS => e
              sourcefile_path.atomic_write(old_contents)
              odie "Unable to fork: #{e.message}!"
            end
          end

          safe_system "git", "fetch", "--unshallow", "origin" if shallow
        end

        safe_system "git", "add", *changed_files
        safe_system "git", "checkout", "--no-track", "-b", branch, "#{remote}/#{remote_branch}" unless args.commit?
        safe_system "git", "commit", "--no-edit", "--verbose",
                    "--message=#{commit_message}",
                    "--", *changed_files
        return if args.commit?

        system_command!("git", args:         ["push", "--set-upstream", remote_url, "#{branch}:#{branch}"],
                               print_stdout: true)
        safe_system "git", "checkout", "--quiet", previous_branch
        pr_message = <<~EOS
          #{pr_message}
        EOS
        user_message = args.message
        if user_message
          pr_message = <<~EOS
            #{user_message}

            ---

            #{pr_message}
          EOS
        end

        begin
          url = create_pull_request(tap_remote_repo, commit_message,
                                    "#{username}:#{branch}", remote_branch, pr_message)["html_url"]
          if args.no_browse?
            puts url
          else
            exec_browser url
          end
        rescue *API::ERRORS => e
          odie "Unable to open pull request: #{e.message}!"
        end
      end
    end
  end

  def self.pull_request_commits(user, repo, pull_request, per_page: 100)
    pr_data = API.open_rest(url_to("repos", user, repo, "pulls", pull_request))
    commits_api = pr_data["commits_url"]
    commit_count = pr_data["commits"]
    commits = []

    if commit_count > API_MAX_ITEMS
      raise API::Error, "Getting #{commit_count} commits would exceed limit of #{API_MAX_ITEMS} API items!"
    end

    API.paginate_rest(commits_api, per_page:) do |result, page|
      commits.concat(result.map { |c| c["sha"] })

      return commits if commits.length == commit_count

      if result.empty? || page * per_page >= commit_count
        raise API::Error, "Expected #{commit_count} commits but actually got #{commits.length}!"
      end
    end
  end

  def self.pull_request_labels(user, repo, pull_request)
    pr_data = API.open_rest(url_to("repos", user, repo, "pulls", pull_request))
    pr_data["labels"].map { |label| label["name"] }
  end

  def self.last_commit(user, repo, ref, version)
    return if Homebrew::EnvConfig.no_github_api?

    output, _, status = Utils::Curl.curl_output(
      "--silent", "--head", "--location",
      "--header", "Accept: application/vnd.github.sha",
      url_to("repos", user, repo, "commits", ref).to_s
    )

    return unless status.success?

    commit = output[/^ETag: "(\h+)"/, 1]
    return if commit.blank?

    version.update_commit(commit)
    commit
  end

  def self.multiple_short_commits_exist?(user, repo, commit)
    return false if Homebrew::EnvConfig.no_github_api?

    output, _, status = Utils::Curl.curl_output(
      "--silent", "--head", "--location",
      "--header", "Accept: application/vnd.github.sha",
      url_to("repos", user, repo, "commits", commit).to_s
    )

    return true unless status.success?
    return true if output.blank?

    output[/^Status: (200)/, 1] != "200"
  end

  def self.repo_commits_for_user(nwo, user, filter, from, to, max)
    return if Homebrew::EnvConfig.no_github_api?

    params = ["#{filter}=#{user}"]
    params << "since=#{DateTime.parse(from).iso8601}" if from.present?
    params << "until=#{DateTime.parse(to).iso8601}" if to.present?

    commits = []
    API.paginate_rest("#{API_URL}/repos/#{nwo}/commits", additional_query_params: params.join("&")) do |result|
      commits.concat(result.map { |c| c["sha"] })
      if max.present? && commits.length >= max
        opoo "#{user} exceeded #{max} #{nwo} commits as #{filter}, stopped counting!"
        break
      end
    end
    commits
  end

  def self.count_repo_commits(nwo, user, from: nil, to: nil, max: nil)
    odie "Cannot count commits, HOMEBREW_NO_GITHUB_API set!" if Homebrew::EnvConfig.no_github_api?

    author_shas = repo_commits_for_user(nwo, user, "author", from, to, max)
    committer_shas = repo_commits_for_user(nwo, user, "committer", from, to, max)
    return [0, 0] if author_shas.blank? && committer_shas.blank?

    author_count = author_shas.count
    # Only count commits where the author and committer are different.
    committer_count = committer_shas.difference(author_shas).count

    [author_count, committer_count]
  end

  MAXIMUM_OPEN_PRS = 15

  sig { params(tap: T.nilable(Tap)).returns(T::Boolean) }
  def self.too_many_open_prs?(tap)
    # We don't enforce unofficial taps.
    return false if tap.nil? || !tap.official?

    # BrewTestBot can open as many PRs as it wants.
    return false if ENV["HOMEBREW_TEST_BOT_AUTOBUMP"].present?

    odie "Cannot count PRs, HOMEBREW_NO_GITHUB_API set!" if Homebrew::EnvConfig.no_github_api?

    query = <<~EOS
      query {
        viewer {
          login
          pullRequests(first: 100, states: OPEN) {
            nodes {
              headRepositoryOwner {
                login
              }
            }
            pageInfo {
              hasNextPage
            }
          }
        }
      }
    EOS
    graphql_result = API.open_graphql(query)
    puts

    github_user = graphql_result.dig("viewer", "login")
    odie "Cannot count PRs, cannot get GitHub username from GraphQL API!" if github_user.blank?

    # BrewTestBot can open as many PRs as it wants.
    return false if github_user.casecmp("brewtestbot").zero?

    prs = graphql_result.dig("viewer", "pullRequests", "nodes")
    more_graphql_data = graphql_result.dig("viewer", "pullRequests", "pageInfo", "hasNextPage")
    return false if !more_graphql_data && prs.length < MAXIMUM_OPEN_PRS

    homebrew_prs_count = graphql_result.dig("viewer", "pullRequests", "nodes").count do |pr|
      pr["headRepositoryOwner"]["login"] == "Homebrew"
    end
    return true if homebrew_prs_count >= MAXIMUM_OPEN_PRS
    return false unless more_graphql_data
    return false if tap.nil?

    url = "#{API_URL}/repos/#{tap.full_name}/issues?state=open&creator=#{github_user}"
    rest_result = API.open_rest(url)
    repo_prs_count = rest_result.count { |issue_or_pr| issue_or_pr.key?("pull_request") }

    repo_prs_count >= MAXIMUM_OPEN_PRS
  end
end
