# cpphycis-engine
kompilacja dla macos (wymagane wczesniej zainstalowanie raylib przez brew)
g++ main.cpp -std=c++17 -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL -o silnik_dod
