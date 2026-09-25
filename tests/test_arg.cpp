#include <QString>
#include <iostream>
int main() {
    uint8_t a = 127;
    QString s = QString("%1").arg(a);
    std::cout << "String length: " << s.length() << " First char code: " << (int)s[0].unicode() << "\n";
}
