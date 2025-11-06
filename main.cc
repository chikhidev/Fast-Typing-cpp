
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <climits>
#include <string.h>
#include <vector>
#include <chrono>
#include <iomanip>

#define success 0
#define error 1
#define read_size 125
#define MAX_FILE_LINES 200
#define MAX_SCORE 5000

#define RESET "\e[0;00m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"

#define DATA_FILE "random_phrases"
#define TRACK_FILE "tracking"
#define TOTAL_ROUNDS 10
#define LOG false

struct bools
{
    bool stop: 1;
    bool fillable: 1;
    bool unused_00: 1;
    bool unused_01: 1;
};

struct score_tracking
{
    int lines;
    unsigned int score;
    size_t total_words;
    float total_time_took;
    std::vector<std::string> logs;
};




bool
miss_cased_character
(const char c, const char og)
{
    return (c == (og - 32) || c == (og + 32));
}



size_t
proccess_single_word
(struct bools *bools_, struct score_tracking *track,
     const std::string& expected, std::string& user_input,
     std::stringstream& diff)
{
    (void)bools_;
    (void)track;
    (void)expected;
    (void)user_input;
    (void)diff;

    size_t correct = 0;
    size_t expected_size = expected.size();
    size_t user_input_size = user_input.size();

    if (expected_size > 0 && expected.c_str()[expected_size - 1] == '\n')
        expected_size--;
    if (user_input_size > 0 && user_input.c_str()[expected_size - 1] == '\n')
        user_input_size--;

    size_t i = 0;
    for (; i < user_input_size; i++)
    {
        if (user_input.c_str()[i] == expected.c_str()[i])
        {
            diff << GREEN << user_input.c_str()[i];
            correct++;
        }
        else if (miss_cased_character(user_input.c_str()[i], expected.c_str()[i]))
            diff << YELLOW << user_input.c_str()[i];
        else
            diff << RED << user_input.c_str()[i];
    }

    for (; i < expected_size; i++)
        diff << RED << user_input.c_str()[i];

    diff << " ";

    if (LOG)
    {
        std::stringstream log;
        log << correct << " correct in word: " << expected;
        track->logs.push_back(log.str());
    }

    return correct;
}


void
print_summary_and_save
(struct score_tracking *track)
{
    std::stringstream summary;
    time_t now = time(NULL);
    std::tm* local = std::localtime(&now);

    if (local)
        summary << "Summary <" << std::put_time(local, "%Y/%m/%d %H:%M:%S") << ">" << std::endl;
    else
        summary << "Summary:" << std::endl;

    summary << "N of lines:\t" << track->lines << std::endl;
    summary << "Total words:\t" << track->total_words << std::endl;

    float time_to_min = track->total_time_took / 60.00f;
    summary << "Total time:\t" << time_to_min << " min" << std::endl;

    float wpm = track->total_words / time_to_min;

    summary << "Wpm:\t\t" << wpm  << "\n\n" << std::endl;

    std::cout << summary.str();

    FILE *tracking_file = fopen(TRACK_FILE, "a");
    if (!tracking_file)
    {
        std::cout << RED << "FAILED TO OPEN tracking file!" << std::endl;
        return;
    }

    if (fwrite(summary.str().c_str(), summary.str().size(), 1, tracking_file) < 0)
        std::cerr << "Failed to write summary in " << TRACK_FILE << std::endl;

    fclose(tracking_file);
}


void
take_user_input
(struct bools *bools_, struct score_tracking *track, const std::string& line)
{
    size_t line_len = line.size();
    std::stringstream diff;

    std::cout << YELLOW << line << RESET;
    if (line[line_len - 1] != '\n')
        std::cout << std::endl;
    else
        line_len--;

    std::string input;
    time_t line_start_typing_time = time(NULL);
    getline(std::cin, input);
    time_t line_end_typing_time = time(NULL);

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

    line_len -= (expected_words.size() - 1);

    std::vector<std::string> user_words;

    line_stream.clear();
    line_stream.seekg(0);
    line_stream.str(input);

    int i = 0;
    size_t correct_sum = 0;
    while (std::getline(line_stream, word, ' '))
    {
        size_t got = proccess_single_word(bools_, track, expected_words[i], word, diff);
        correct_sum += got;

        size_t expected_len = expected_words[i].size();
        expected_len -= (expected_words[i].c_str()[expected_len - 1] == '\n');

        if (got == expected_len)
            track->total_words++;
       
        if (LOG)
        {
            std::stringstream ss;
            ss << "Expected " << word.size() << " for " << word;
            track->logs.push_back(ss.str());
        }

        i++;
    }

    double line_took_time = (line_end_typing_time - line_start_typing_time);

    system("clear");

    diff << RESET;
    std::cout << line << std::endl;
    std::cout << diff.str() << std::endl;
    std::cout << "Correct: " << correct_sum << "/" << line_len << std::endl;
    std::cout << "Time took: " << line_took_time << " s" << std::endl;
    std::cout << std::endl;
    
    track->lines++;
    track->total_time_took += line_took_time;
}

int
main
()
{

    FILE *input = fopen(DATA_FILE, "r");
    if (!input) return error;

    std::stringstream file_content("");

    char *line;
    size_t line_len = 0;

    struct score_tracking track;
    struct bools bools_;

    memset(&track, 0, sizeof(track));
    memset(&bools_, 0, sizeof(bools_));

    system("clear");

    size_t lines_count = 0;
    std::vector<std::string> lines_store;

    while (getline(&line, &line_len, input) > 0) {
        lines_store.push_back(line);
        lines_count++;
    }

    fclose(input);

    std::srand(time(NULL));

    for (int i = 0; i < TOTAL_ROUNDS; i++)
    {
        std::cout << "Preparing a phrase..." << std::endl;
        size_t random_idx = std::rand() % lines_count;
        if (bools_.stop) break;
        take_user_input(&bools_, &track, lines_store[random_idx]);
    }

    print_summary_and_save(&track);

    for (size_t i = 0; LOG && i < track.logs.size(); i++)
        std::cout << track.logs[i] << std::endl;

    return success;
}

