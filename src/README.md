# Trade-Matching Engine Project V 1.0

Hello there!
My name is Eric Vaz, and i am exited to share my Trade-Matching-Engine project. My goal with this project was to get clear on the inner workings of a Matching engine,so over the span of 2025's summer i decided to implement my own!
The Engine was fully implemented in **c++17**.


### Key features

The V1 has built-in support for :
- Market and Limit Orders
- User Registration and Data interaction

### Upcomming features 
- Graphical User Interface for the
client side

- Support for Future contracts
- Encryption for Client-Server Communication

### Usage

Supported Platform : **Linux**
Dependencies:
- **Sqlite3**

**Instalation guide**:

**Step 1: Install sqlite** 
    Installing sqlite can be done through the command line on any Debian based operating system with:
        `sudo apt-get install sqlite3`

**Step 2: Clone the repository**
    `git clone https://github.com/thricvz/Trade-Matching-Engine.git`

**Step 3:Compile the source files**

Move to the recently cloned directory:
`cd Trade-Matching-Engine`

Generate the build Files:
    `cmake -S src -B build`

     
Compile the project:
    `cmake --build build`


**Congratulations!**
You have successfully compiled the project!
You will find both the server and client executables in the **build directory**.