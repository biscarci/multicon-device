#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAX_MESSAGE_LENGTH 1000
#define LOGGER_DEBUG  0
#define LOGGER_INFO   1
#define LOGGER_WARN   2
#define LOGGER_ERROR  3
#define LOGGER_FATAL  4

static int min_log_level = 0; 


void system_logger(int level, char* module, const char* fmt, ...);
void logger_init();