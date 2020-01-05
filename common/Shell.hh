#ifndef _SHELL_HH_
#define _SHELL_HH_

/**
 * Shell.hh
 * Defines a basic class for interactive shell
 */

#include <functional>
#include <map>
#include <unordered_map>
#include <vector>
#include "common.hh"

#define SHELL_REGISTER_COMMAND_START \
    protected: \
        virtual void initialize() { \

#define SHELL_REGISTER_COMMAND_END \
        }

#define REGISTER_SHELL_COMMAND(cmd, cargc, func, hm) \
            this->registerCommand(cmd, cargc, func, hm);

#define DECLARE_SHELL_EXECUTION_METHOD(name)    \
    protected: \
        static int name (Shell *, const Args_t &);

namespace common
{
class Shell;

/**
 * class Shell
 * The basic class of an interactive shell, support help and exit
 */
class Shell
{
    protected:
        using Cmd_t     = std::string;
        using Args_t    = std::vector<std::string>;
        using Func_t    = std::function<int(Shell *, const Args_t &)>;

        struct CmdInfo
        {
            int     argc;
            Func_t  func;
            std::string helpmsg;
        };
        enum Color_t {NORMAL = 0, RED, GREEN, YELLOW, BLUE, MAG, CYAN };

    protected:
        std::unordered_map<Cmd_t, CmdInfo> cmds_;
        std::string env;    // used as prefix

    protected:
        void changeColor(Color_t color);
        void execute(const Cmd_t &cmd, const Args_t &args);
        void registerCommand(const Cmd_t &cmd, int argc, const Func_t &func, 
                const std::string &helpmsg = "");

        DECLARE_SHELL_EXECUTION_METHOD(help);
        DECLARE_SHELL_EXECUTION_METHOD(quit);

        SHELL_REGISTER_COMMAND_START
            REGISTER_SHELL_COMMAND("help", 1, help, "show help message, usage: help [command]");
            REGISTER_SHELL_COMMAND("quit", 0, quit, "quit the shell");
        SHELL_REGISTER_COMMAND_END

    public:
        Shell()
        {
            Shell::initialize();
            this->env = "BS";
        }

        virtual void start();
        virtual std::string getName() const { return "Basic Shell";}
        virtual ~Shell() = default;
};


};

#endif
