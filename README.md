<h1 align="center">minishell_42_1337 </h1>
<p align="center">
  <a href="https://github.com//mohimi-coder/minishell">
    <img src="https://raw.githubusercontent.com/mohimi-coder/1337_badges/refs/heads/master/minishellm.png" alt="42 Badge">
  </a>
</p>

<!--HEADER-->
<h1 align="center"> Minishell | 
 <picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://cdn.simpleicons.org/42/white">
  <img alt="42" width=40 align="center" src="https://cdn.simpleicons.org/42/Black">
 </picture>
 Cursus 
  <img alt="Complete" src="https://raw.githubusercontent.com/Mqxx/GitHub-Markdown/main/blockquotes/badge/dark-theme/complete.svg">
</h1>
<!--FINISH HEADER-->


#minishell

🚀 This project was one of the most challenging yet rewarding experiences in the 42 curriculum. It involved handling numerous test cases and learning a wide range of topics, 
from software architecture and system calls to team coordination and task distribution.

⚡ Initially, building a shell that mimics Bash seemed intimidating. Our prior knowledge of shells was limited to basic usage, and we had no idea how they worked internally.

➥ Creating a shell from scratch was a significant challenge, but it highlighted one of the core aspects we love about the 42 pedagogy: 
constantly facing new challenges and learning opportunities, from the early days of the Piscine (42 selection phase) up to now.

💥 Minishell is a substantial project, especially for beginners, and it required careful planning and research to avoid major redesigns later—although We still ended up making significant changes.

👩🏻‍💻🧑🏻‍💻 Since this was a team project, We needed to find a partner. After forming a team of two, we started coordinating tasks and working together.

🛑 The shell has two main parts: parsing (processing user input) and execution (running the parsed commands). 
Through this project, We not only improved my technical skills but also learned valuable lessons in teamwork and project management.

#</> Anyway, let’s have a project overview…

✔ We are required to build a mini shell (command-line interpreter) that mimics the bash, hence the name it wouldn’t be doing all the work that bash does, but the basic functionality:

    ► The shell will work only in interactive mode (no scripts, i.e. the executable takes no arguments)
    
    ► Run simple commands with absolute, relative path (e.g. /bin/ls, ../bin/ls)
    
    ► Run simple commands without a path (e.g. ls, cat, grep, etc…)
    
    ► Have a working history (you can navigate through commands with up/down arrows)
    
    ► Implement pipes (|)
    
    ► Implement redirections (<, >, >>)
    
    ► Implement the here-doc (<<)
    
    ► Handle double quotes ("") and single quotes (''), which should escape special characters, beside $ for double quotes.
    
    ► Handle environment variables ($ followed by a sequence of characters).
    
    ► Handle signals like in bash (ctrl + C, ctrl + \, ctrl + D).
    
#👉🏼Implement the following built-ins:

    ➜ echo (option -n only)
    
    ➜ exit
    
    ➜ env (with no options or arguments)
    
    ➜ export (with no options)
    
    ➜ unset (with no options)
    
    ➜ cd
    
    ➜ pwd
    
#⚠And for the bonus part (optional, for us we didn't do it)

    ➤ handle && and || with the parenthesis () for priority.
    
    ➤ handle * wildcards for the current working directory.

#😵‍💫OMG! Where to start?! That was our reaction when we first read the assignment.

After researching how Bash works, We discovered that its implementation is divided into two main parts:


  #🌟 Handles user input and interaction, such as commands and signals.
  
  #🌟 Manages the execution of commands.
  
◼ In the first part, we need to handle two main things: commands (user input as text) and signals (like Ctrl+C). 

◼ We decided to focus on handling user input first and postpone signal handling to avoid unnecessary complexity at the start.

✏️ Our first task was to figure out how the shell processes user input. We realized there must be a systematic way to handle these lines. Many of our peers were tempted to hard-code the parsing and handle cases naively. 

𑁍We felt the same temptation but chose to step out of our comfort zone and learn something new.

╰┈➤By taking this approach, We aimed to create a more robust and flexible shell, gaining deeper insights into how shells work and improving our programming skills along the way.

![idea](https://github.com/user-attachments/assets/c151966d-2a1a-4213-98b0-b600c1481f69)


