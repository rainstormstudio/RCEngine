#include "RCEngine.hpp"

class Demo : public RCEngine {
    double t;
    Uint8 calcR(int n) {
        n += t;
        int x = round(t) + n;
        x += 255;
        x = x % 500;
        return static_cast<Uint8>(abs(255 - x));
    }
    Uint8 calcG(int n) {
        n += t;
        int x = round(t) + n;
        x = x % 500;
        return static_cast<Uint8>(abs(255 - x));
    }
    Uint8 calcB(int n) {
        n += t;
        int x = round(t) + n;
        x += 128;
        x = x % 500;
        return static_cast<Uint8>(abs(255 - x));
    }
public:
    Demo() {
        windowTitle = "Demo";
    }

    bool start() override {
        t = 0.0f;
        return true;
    }

    bool update(double deltaTime) override {
        t += deltaTime * 120.0;
        if (t > 500.0f) {
            t = 0.0f;
        }
        for (int x = 0; x < cellCols; x ++) {
            for (int y = 0; y < cellRows; y ++) {
                draw(x, y, ' ', 
                    {255, 255, 255, 255}, 
                    {static_cast<Uint8>(round(calcR(x * (cellRows - y)))), 
                    static_cast<Uint8>(round(calcG(y * (cellCols - x)))), 
                    static_cast<Uint8>(round(calcB(x * y))), 255});
            }
        }
        write(0, 0, "hello world");
        return true;
    }
};

int main() {
    Demo test;
    if (test.createConsole()) {
        test.init();
    }
    
    return 0;
}