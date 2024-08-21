#include <fstream>
#include <regex>
#include <string>
#include <iostream>
#include <unordered_map>
#include <utility> 
#include <ctime>

// Creating a regular expression robot/chatbot
class RegexBot {
/*
// Creating the public features, which include running the robot
// along with creating a file to log the answers given by the user
*/
public:
    RegexBot() : speaker_name(""), user_mood("neutral"), last_topic("") {
        log_file.open("conversation_log.txt", std::ios::app); // Open log file in append mode
        if (!log_file.is_open()) {
            std::cerr << "Failed to open log file.\n";
        }
    }

    ~RegexBot() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    void run() {
        on_startup();
        std::string usr_msg = "";

        while (usr_msg != "quit") {
            auto result = take_turn();
            usr_msg = result.first;
            bot_reply = result.second;
        }
    }

/*
// Private features, which include the bots functionality
*/
private:
    std::string speaker_name; // stores the users name
    std::string bot_reply; // generates the reply
    std::string user_mood; // stores how the user is feeling
    std::string last_topic; // stores the previou topic of conversation
    std::ofstream log_file;
    std::unordered_map<std::string, std::string> activities; // stores the activities the user enjoys

    /*
    // Greets the user appropriately
    */
    std::string get_greeting() {
        std::time_t now = std::time(0);
        std::tm *ltm = std::localtime(&now);
        int hour = ltm->tm_hour;

        if (hour < 12) return "Good morning";
        if (hour < 17) return "Good afternoon";
        return "Good evening";
    }

    /*
    // Makes the reply based on the regular expression
    */
    std::string make_reply(const std::string &text) {
        std::smatch m;
        std::regex reg1(R"(\bI (hate|love|like) (?!my )(\w+))"); // What they hate, love, like
        std::regex reg2(R"(\bI (hate|love|like) my (\w+))");
        std::regex reg3(R"(^[Bb]ecause ((she|he) is|they are) ([\w\s]+))"); // Reason for hate/love/like 
        std::regex reg4(R"(^[Bb]ecause it is ([\w\s]+))");
        std::regex reg5(R"(.*\b(hello|hi)\b.*)", std::regex_constants::icase); // Detects greeting
        std::regex reg7(R"(.*\b(favorite|best)\b.*)", std::regex_constants::icase); // Favorite activities detection
        std::regex reg8(R"(.*\b(why|what)\b.*)", std::regex_constants::icase); // User asking "why" or "what"
        std::regex reg9(R"(I am feeling (sad|happy|angry|excited|frustrated|anxious|joyful|content) because (.*))", std::regex_constants::icase); // Reason for emotion
        std::regex reg6(R"(.*\b(sad|happy|angry|excited|frustrated|anxious|joyful|content)\b.*)", std::regex_constants::icase); // Mood detection
        std::regex reg10(R"(\b(yes|no)\b)", std::regex_constants::icase); // Yes/No responses

        // Check for specific reasons for emotions first
        if (std::regex_search(text, m, reg9)) {
            std::string emotion = m.str(1);
            std::string reason = m.str(2);
            std::string response;
            if (emotion == "sad") {
                response = "I’m sorry you’re feeling sad because " + reason + ". Is there anything specific I can do to help or anything that might make you feel better?";
            } else if (emotion == "happy") {
                response = "It's great to hear you're happy because " + reason + ". What else is making you feel good?";
            } else if (emotion == "angry") {
                response = "I understand you're feeling angry because " + reason + ". Would you like to talk more about it?";
            } else if (emotion == "excited") {
                response = "I'm glad you're excited because " + reason + ". What's the most exciting part for you?";
            } else if (emotion == "frustrated") {
                response = "I'm sorry you're frustrated because " + reason + ". Is there anything I can do to help?";
            } else if (emotion == "anxious") {
                response = "I understand you're anxious because " + reason + ". Sometimes talking helps. Do you want to share more?";
            } else if (emotion == "joyful") {
                response = "It's wonderful you're feeling joyful because " + reason + ". What else is contributing to your joy?";
            } else if (emotion == "content") {
                response = "It's nice to hear you're content because " + reason + ". Anything else you'd like to share about this feeling?";
            }
            // Replace "I am" with "you are" in the response
            std::regex i_am_regex("I am");
            response = std::regex_replace(response, i_am_regex, "you are");
            return response;
        }
        
        // Check for general mood mentions
        if (std::regex_search(text, m, reg6)) {
            user_mood = m.str(1);
            if (user_mood == "sad") {
                return "I'm sorry to hear that you're feeling sad. Can you tell me more about what's going on? Sometimes sharing can help.";
            } else if (user_mood == "happy") {
                return "That's great to hear that you're feeling happy! What's been making you feel this way?";
            } else if (user_mood == "angry") {
                return "I'm sorry you're feeling angry. Do you want to talk about what's upsetting you?";
            } else if (user_mood == "excited") {
                return "It's wonderful that you're feeling excited! What's got you feeling this way?";
            } else if (user_mood == "frustrated") {
                return "I understand that you're feeling frustrated. Would you like to share what's causing this frustration?";
            } else if (user_mood == "anxious") {
                return "I'm sorry you're feeling anxious. Sometimes it helps to talk about what's on your mind. Would you like to share?";
            } else if (user_mood == "joyful") {
                return "I'm glad to hear you're feeling joyful! What's been bringing you so much joy?";
            } else if (user_mood == "content") {
                return "It's nice to hear that you're feeling content. Is there anything special that's making you feel this way?";
            }
        }

        // Check for activity-related responses
        if (std::regex_search(text, m, reg1)) {
            std::string sentiment = m.str(1);
            std::string activity = m.str(2);
            activities[activity] = sentiment; // Store the activity and sentiment
            return "Why do you " + sentiment + " " + activity + "?";
        } else if (std::regex_search(text, m, reg2)) {
            std::string sentiment = m.str(1);
            std::string activity = m.str(2);
            activities[activity] = sentiment; // Store the activity and sentiment
            return "Why do you " + sentiment + " your " + activity + "?";
        } else if (std::regex_search(text, m, reg3)) {
            return "That is interesting to hear that they are " + m.str(3) + "!";
        } else if (std::regex_search(text, m, reg4)) {
            return "You are right, that activity is " + m.str(1) + "!";
        } else if (std::regex_search(text, m, reg5)) {
            return get_greeting() + "! How can I assist you today?";
        } else if (std::regex_search(text, m, reg7)) {
            return "I would love to hear more about your favorite activities. What else do you enjoy?";
        } else if (std::regex_search(text, m, reg8)) {
            if (user_mood == "sad" || user_mood == "happy" || user_mood == "angry" || user_mood == "excited" || user_mood == "frustrated" || user_mood == "anxious" || user_mood == "joyful" || user_mood == "content") {
                return "It's okay to feel this way. If you feel comfortable, tell me more about what's on your mind. Sometimes it helps to talk about it.";
            }
        }

        // Handle Yes/No responses
        if (std::regex_search(text, m, reg10)) {
            std::string answer = m.str(1);
            if (answer == "yes") {
                if (last_topic == "mood") {
                    return "I'm glad to hear that! Is there anything else you'd like to discuss or share?";
                }
                return "Yes, what would you like to discuss next?";
            } else if (answer == "no") {
                if (last_topic == "mood") {
                    return "I understand. If there's anything you'd like to share later, I'm here to listen.";
                }
                return "No worries! Feel free to ask me anything else.";
            }
        }

        // Respond based on stored activities and sentiments
        for (const auto& [activity, sentiment] : activities) {
            if (text.find(activity) != std::string::npos) {
                return "I remember you " + sentiment + " " + activity + ". Is there anything else you want to share about it?";
            }
        }

        // Dynamic and engaging response if no patterns matched
        if (text.find("thanks") != std::string::npos || text.find("thank you") != std::string::npos) {
            return "You're welcome! Is there anything else you'd like to talk about?";
        }

        return "Hmm, I didn't quite catch that. Could you tell me more about it?";
    }

    // Prints to user and to conversation log
    void bot_print(const std::string &msg) {
        std::cout << "\nBot: " << msg << "\n\n";
        if (log_file.is_open()) {
            log_file << "Bot: " << msg << "\n";
        }
    }

    // logs user response and then generates response
    std::pair<std::string, std::string> take_turn() {
        std::cout << "You: ";
        std::string usr_msg;
        std::getline(std::cin, usr_msg);
        if (log_file.is_open()) {
            log_file << "You: " << usr_msg << "\n";
        }
        bot_reply = make_reply(usr_msg);
        bot_print(bot_reply);
        return std::make_pair(usr_msg, bot_reply);
    }

    void on_startup() {
        std::cout << "Bot: What is your name?\n\nYou: ";
        std::getline(std::cin, speaker_name);
        bot_print(get_greeting() + ", " + speaker_name + "! Type 'quit' at any time to end our conversation.");
        bot_print("Tell me about something you like, love, or hate. Or share how you're feeling today:");
        last_topic = "initial"; // Set initial topic
    }
};

int main() {
    RegexBot bot;
    bot.run();
    return 0;
}
