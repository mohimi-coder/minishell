#minishell

🚀 This project was one of the most challenging yet rewarding experiences in the 42 curriculum. It involved handling numerous test cases and learning a wide range of topics, 
from software architecture and system calls to team coordination and task distribution.

⚡ Initially, building a shell that mimics Bash seemed intimidating. My prior knowledge of shells was limited to basic usage, and I had no idea how they worked internally.

➥ Creating a shell from scratch was a significant challenge, but it highlighted one of the core aspects I love about the 42 pedagogy: 
constantly facing new challenges and learning opportunities, from the early days of the Piscine (42 selection phase) up to now.

💥 Minishell is a substantial project, especially for beginners, and it required careful planning and research to avoid major redesigns later—although I still ended up making significant changes.

👨🏻‍💻👩🏽‍💻 Since this was a team project, I needed to find a partner. After forming a team of two, we started coordinating tasks and working together.

🛑 The shell has two main parts: parsing (processing user input) and execution (running the parsed commands). 
Through this project, I not only improved my technical skills but also learned valuable lessons in teamwork and project management.

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
    
#⚠And for the bonus part (optional, for me i din't do it)

    ➤ handle && and || with the parenthesis () for priority.
    
    ➤ handle * wildcards for the current working directory.

#😵‍💫OMG! Where to start?! That was my reaction when I first read the assignment.

After researching how Bash works, I discovered that its implementation is divided into two main parts: the front-end and the back-end.

#Front-End: 

  🌟 Handles user input and interaction, such as commands and signals.
  
#Back-End:
  🌟 Manages the execution of commands.
  
◼ In the front-end, we need to handle two main things: commands (user input as text) and signals (like Ctrl+C). 

◼ I decided to focus on handling user input first and postpone signal handling to avoid unnecessary complexity at the start.

✏️ My first task was to figure out how the shell processes user input. I realized there must be a systematic way to handle these lines. Many of my peers were tempted to hard-code the parsing and handle cases naively. 

𑁍I felt the same temptation but chose to step out of my comfort zone and learn something new.

╰┈➤By taking this approach, I aimed to create a more robust and flexible shell, gaining deeper insights into how shells work and improving my programming skills along the way.

![idea](https://github.com/user-attachments/assets/c151966d-2a1a-4213-98b0-b600c1481f69)


