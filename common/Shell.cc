#include <iostream>
#include <stdexcept>
#include "Shell.hh"

namespace common
{

int Shell::help(Shell *ctx, const Args_t &args)
{
    if(args.size() == 0)
    {
        /* show total help message */
        std::cout<<"Shell version: "<< ctx->getName()<<"; Executable commands: "<<std::endl;
        for(auto &cmd : ctx->cmds_)
        {
            ctx->changeColor(YELLOW);
            std::cout<<"  "<<cmd.first;
            ctx->changeColor(NORMAL);
            std::cout<<": "<<cmd.second.helpmsg<<std::endl;
        }
    }
    else
    {
        auto target_cmd = args[0];
        if(ctx->cmds_.count(target_cmd))
        {
            const auto &cmd = ctx->cmds_.at(target_cmd);
            ctx->changeColor(YELLOW);
            std::cout<<"  "<<target_cmd;
            ctx->changeColor(NORMAL);
            std::cout<<": "<<cmd.helpmsg<<std::endl;
        }
        else std::cout<<"There is no command called: target_cmd"<<std::endl;
    }
    return 0;
}

int Shell::quit(Shell *ctx, const Args_t &args)
{
    SUPPRESS_UNUSED(ctx);
    SUPPRESS_UNUSED(args);
    ctx->changeColor(BLUE);
    std::cout<<"Bye!"<<std::endl;
    ctx->changeColor(NORMAL);
    exit(0);
}

void Shell::registerCommand(const Cmd_t &cmd, int argc, const Func_t &func,
        const std::string &helpmsg)
{
    if(this->cmds_.count(cmd))
        throw std::runtime_error("Shell::registerCommand: duplicated command: " + cmd);
    this->cmds_[cmd] = {argc, func, helpmsg};
}

void Shell::execute(const Cmd_t &cmd, const Args_t &args)
{
    SUPPRESS_UNUSED(args);
    if(this->cmds_.count(cmd) == 0)
    {
        this->changeColor(RED);
        std::cout<<cmd<<": command not found"<<std::endl;
        this->changeColor(NORMAL);
        return;
    }

    auto ci = cmds_.at(cmd);
    int ret = -1;
    try
    {
        ret = ci.func(this,args);
    }
    catch(std::exception &e)
    {
        this->changeColor(RED);
        std::cout<<"While executing command, an exception occurs: "<<e.what()<<std::endl;
        this->changeColor(NORMAL);
    }
    if(ret != 0)
    {
        this->changeColor(YELLOW);
        std::cout<<"Command returned: "<<ret;
        this->changeColor(NORMAL);
        std::cout<<std::endl;
    }
}

void Shell::changeColor(Color_t color)
{
    static std::vector<std::string> colors{
            "\x1B[0m",
            "\x1B[31m",
            "\x1B[32m",
            "\x1B[33m",
            "\x1B[34m",
            "\x1B[35m",
            "\x1B[36m",
    };
    std::cout<<colors[color];
    std::cout.flush();
}

void Shell::start()
{
    std::cout<<"Welcome to "<<this->getName()<<std::endl;
    std::string line;
    while(true)
    {
        this->changeColor(GREEN);
        std::cout<<env;
        this->changeColor(NORMAL);
        std::cout<<"$ ";
        std::getline(std::cin, line);
        auto vec = common::split(line, ' ');
        if(vec.size() == 0) continue;
        Cmd_t cmd = vec[0];
        Args_t arg;
        for(unsigned i=1;i<vec.size();i++) arg.push_back(vec[i]);
        this->execute(cmd, arg);
    }
}

} // namespace common
