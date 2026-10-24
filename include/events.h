#include<string>
#include<typeindex>
#include<unordered_map>
#include<functional>
#include <any>

enum class LogLevel {
	DEBUG,
	INFO,
	WARNING,
	ERROR,	
};

struct LogEvent {
	LogLevel level;
	std::string message;
};


class EventBus {
public:    
	// Subcribes to any type of event T
    template <typename EventType>
    void subscribe(std::function<void(const EventType&)> handler) {
        auto type = std::type_index(typeid(EventType));

        handlers[type].push_back([handler = std::move(handler)](const std::any& eventAny) {        
            handler(std::any_cast<const EventType&>(eventAny));
        });
    }

    // Publishes any type of event T
    template <typename EventType>
    void publish(const EventType& event) const {
        auto type = std::type_index(typeid(EventType));
        auto it = handlers.find(type);

        if (it != handlers.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
        }
    }

private:
    using AnyHandler = std::function<void(const std::any&)>;
    std::unordered_map<std::type_index, std::vector<AnyHandler>> handlers;
};