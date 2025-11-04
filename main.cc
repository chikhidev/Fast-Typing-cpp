#define success 0
#define error 1
#define read_size 125

#define RESET "\e[0;37m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"

#define MAIN main

#include <iostream>
#include <sstream>
#include <ctime>

struct score_tracking
{
    double time_took;
    int lines;
    unsigned int score: 31;
    bool stop: 1;
};


size_t
string_len
(const char *str)
{
    int l = 0;
    while (str[l])
        l++;
    return l;
}

void
take_user_input
(struct score_tracking *track, const char *line)
{
    size_t line_len = string_len(line);
    size_t correct = line_len;
    std::stringstream diff;

    std::cout << line;
    if (line[line_len - 1] != '\n') std::cout << std::endl;

    std::string input;

    getline(std::cin, input);

    if (input.size() == 0)
    {
        track->stop = true;
        return;
    }

    if (input.size() == 1 && input[input.size() - 1] == '\n') return;

    bool wrong = false;
    
    for (size_t i = 0; i < line_len; i++)
    {
        if (line[i] == '\n') break;
        wrong = (line[i] != input[i]);
        correct -= wrong;
        diff << (wrong ? RED : GREEN) << line[i];
    }

    diff << RESET;
    std::cout << diff.str() << std::endl;
    if (track->score < 2147483647)
        track->score += correct;
    track->lines++;
    std::cout << correct << "/" << line_len << " Correct\n" << std::endl;
}

int
main
(int ac,char **av)
{
    if (ac != 2) return error;

    FILE *input = fopen(av[1], "r");
    if (!input) return error;

    std::stringstream file_content("");

    char *line;
    size_t line_len = 0;

    struct score_tracking track;
    track.time_took = 0.0;
    track.lines = 0;
    track.stop = false;

    std::clock_t start = std::clock();

    while (getline(&line, &line_len, input) > 0) {
        if (track.stop) break;
        take_user_input(&track, line);
    }

    track.time_took = double(std::clock() - start) / CLOCKS_PER_SEC;
    std::cout << "Summary:" << std::endl;
    std::cout << "N of lines: " << track.lines << std::endl;
    std::cout << "Time took: " << track.time_took << " MS" << std::endl;
    std::cout << "Score: " << (track.time_took > 0 ? track.score / track.time_took : 0) << std::endl;

    return success;
}

