#include "common.hh"
using std::cout;
using std::endl;

int main(){
    cout << SizeClass::roundUp(10) << endl;
    cout << SizeClass::roundUp(17) << endl;    
    cout << SizeClass::roundUp(35) << endl;    
    cout << SizeClass::roundUp(129) << endl;

    cout << SizeClass::index(4) << endl;
    cout << SizeClass::index(144) << endl;
    cout << SizeClass::index(176) << endl;

    return 0;    
}