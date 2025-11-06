
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <climits>
#include <string.h>
#include <vector>

#define success 0
#define error 1
#define read_size 125
#define MAX_FILE_LINES 200
#define MAX_SCORE 5000

#define RESET "\e[0;00m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"



struct bools
{
    bool stop: 1;
    bool fillable: 1;
    bool unused_00: 1;
    bool unused_01: 1;
};

struct score_tracking
{
    double time_took;
    int lines;
    unsigned int score;
    int last_time_correct;
    double last_line_time_taken;
};


size_t
string_len
(const char *str)
{
    int l = 0;
    while (str[l] && str[l] != '\n')
        l++;
    return l;
}


void
print_current_summary
(struct bools *bools_, struct score_tracking *track, double& took_line_time, double& time_advencement)
{
    (void)bools_;
    system("clear");
    if (track->score == 0) return;

    double score_percentage = (double(track->score) / MAX_SCORE) * 100;

    size_t i = 0;
    for (; i < 100; i++)
    {
        if (i < score_percentage)
            std::cout << YELLOW << "-";
        else
            std::cout << RESET << " ";
    }
    std::cout << " " << score_percentage << "%" << RESET << std::endl;

    if (track->last_line_time_taken != 0 && took_line_time > track->last_line_time_taken)
    {
        time_advencement = double(took_line_time / track->last_line_time_taken) * 100;
        std::cout << RED << "Speed downgrade from last time: " << time_advencement << "%" << RESET << std::endl;
    }
    else if (track->last_time_correct > 0)
    {
        time_advencement = double(track->last_line_time_taken / took_line_time) * 100;
        std::cout << GREEN << "Speed upgrade from last time: " << time_advencement << "%" << RESET << std::endl;
    }


}


void proccess_single_word
(struct bools *bools_, struct score_tracking *track,
     const std::string& expected, std::string& user_input,
     std::stringstream& diff)
{
    (void)bools_;
    (void)track;
    (void)expected;
    (void)user_input;
    (void)diff;
}


void
take_user_input
(struct bools *bools_, struct score_tracking *track, const char *line)
{
    size_t line_len = string_len(line);
    std::stringstream diff;

    std::cout << line;
    if (line[line_len - 1] != '\n') std::cout << std::endl;

    std::string input;
    std::clock_t line_start_typing_time = std::clock();
    getline(std::cin, input);
    std::clock_t line_end_typing_time = std::clock();

    if (input.size() == 0)
    {
        bools_->stop = true;
        return;
    }

    if (input.size() == 1 && input[input.size() - 1] == '\n') return;

    // AT THIS POINT USER INPUT
    std::vector<std::string> expected_words;

    std::stringstream line_stream(line);
    std::string word;

    while (std::getline(line_stream, word, ' '))
        expected_words.push_back(word);

    std::vector<std::string> user_words;

    line_stream.clear();
    line_stream.seekg(0);
    line_stream.str(input);

    int i = 0;
    while (std::getline(line_stream, word, ' '))
    {
        proccess_single_word(bools_, track, expected_words[i], word, diff);
    } 


    (void)track;
    (void)line_start_typing_time;
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
    struct bools bools_;

    memset(&track, 0, sizeof(track));
    memset(&bools_, 0, sizeof(bools_));

    system("clear");

    std::clock_t start = std::clock();

    while (getline(&line, &line_len, input) > 0) {
        if (bools_.stop) break;
        take_user_input(&bools_, &track, line);

    }

    track.time_took = double(std::clock() - start) / CLOCKS_PER_SEC;
    std::cout << "Summary:" << std::endl;
    std::cout << "N of lines: " << track.lines << std::endl;
    std::cout << "Time took: " << track.time_took << " MS" << std::endl;
    std::cout << "Score: " << (track.time_took > 0 ? track.score / track.time_took : 0) << std::endl;

    return success;
}

