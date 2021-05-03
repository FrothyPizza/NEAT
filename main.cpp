

#include "Application.h"






int main() {
    srand(static_cast<unsigned int>(time(NULL)));
   
    std::vector<ConnectionHistory> a;
    Genome start{ 1, 2 };
    //for(int i = 0; i < 100; ++i) start.mutate(a);
    start.mutate(a);

    Genome next{ 1, 2 };
    next.mutate(a);


    
    Application game{};

    sf::Clock clock{};
    clock.restart();

    int fps = 30;

    while (game.running()) {
        game.update();

        if (clock.getElapsedTime().asMilliseconds() > 1000 / fps) {
            game.render(); 
        }
    }


    return 0;
}