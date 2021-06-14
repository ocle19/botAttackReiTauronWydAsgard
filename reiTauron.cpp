#include <iostream>
#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#include <fstream>  
#include <vector>
using namespace std;

DWORD pid;
CHAR target[250];
boolean nasceu = false;
CHAR nickname[250];
CHAR coodenadasReiTauron[250];
float playerX;
float playerY;
int reiX;
int reiY;
int modoAtaque = 0;
int pocaoHp = 30;
int racaoPet = 30;
HANDLE pHandle;
DWORD rf_client;
DWORD rf_client2;
DWORD baseAddress;
DWORD address;
DWORD addressCoordenadasReiTauron;
DWORD addressNickname;
DWORD addressPlayerX;
DWORD addressPlayerY;
DWORD addressModoAtaque;
DWORD addressPocaoHp;
DWORD addressRacaoPet;
//Node* node;

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

DWORD GetModuleBase(const wchar_t* ModuleName, DWORD ProcessId) {
    // Esta estrutura contém muitas coisas boas sobre um módulo
    MODULEENTRY32 ModuleEntry = { 0 };
    // Obtenha um instantâneo de todos os módulos no processo especificado
    HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessId);

    if (!SnapShot)
        return NULL;

    // Você tem que inicializar o tamanho, caso contrário não funcionará
    ModuleEntry.dwSize = sizeof(ModuleEntry);

    // Obtenha o primeiro módulo do processo
    if (!Module32First(SnapShot, &ModuleEntry))
        return NULL;

    do {
        // Verifique se o nome do módulo corresponde ao que estamos procurando
        if (!wcscmp(ModuleEntry.szModule, ModuleName)) {
            // Em caso afirmativo, feche o identificador de instantâneo e retorne o endereço de base
            CloseHandle(SnapShot);
            return (DWORD)ModuleEntry.modBaseAddr;
        }
        // Pegue o próximo módulo no instantâneo
    } while (Module32Next(SnapShot, &ModuleEntry));

    // Não foi possível encontrar o módulo especificado, então retorne NULL
    CloseHandle(SnapShot);
    return NULL;
}

BOOL IsProcessRunning(DWORD pid)
{
    HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
    DWORD ret = WaitForSingleObject(process, 0);
    CloseHandle(process);
    return ret == WAIT_TIMEOUT;
}

void cls(HANDLE hConsole)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT scrollRect;
    COORD scrollTarget;
    CHAR_INFO fill;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    // Scroll the rectangle of the entire buffer.
    scrollRect.Left = 0;
    scrollRect.Top = 0;
    scrollRect.Right = csbi.dwSize.X;
    scrollRect.Bottom = csbi.dwSize.Y;

    // Scroll it upwards off the top of the buffer with a magnitude of the entire height.
    scrollTarget.X = 0;
    scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

    // Fill with empty spaces with the buffer's default text attribute.
    fill.Char.UnicodeChar = TEXT(' ');
    fill.Attributes = csbi.wAttributes;

    // Do the scroll
    ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);

    // Move the cursor to the top left corner too.
    csbi.dwCursorPosition.X = 0;
    csbi.dwCursorPosition.Y = 0;

    SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

double GetDistance(int x1, int y1, int x2, int y2)
{
    return sqrt(float(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1))));
}
vector<string> split(string str, char delimiter = ',')
{
    vector<string> ret;
    if (str.empty())
    {
        ret.push_back(string(""));
        return ret;
    }

    unsigned i = 0;
    string strstack;
    while (!(str.empty()) && (str[0] == delimiter)) { str.erase(0, 1); }
    reverse(str.begin(), str.end());
    while (!(str.empty()) && (str[0] == delimiter)) { str.erase(0, 1); }
    reverse(str.begin(), str.end());
    while (!str.empty())
    {
        ret.push_back(str.substr(i, str.find(delimiter)));
        str.erase(0, str.find(delimiter));
        while (!(str.empty()) && (str[0] == delimiter)) { str.erase(0, 1); }
    }

    return ret;
}

int main()
{
    setlocale(LC_ALL, "Portuguese");
    //node = new Node();
    std::wstring windowName;
    std::string newWindowName;

    std::wcout << "Entre no canal desejado e fique dentro do deserto para iniciar.\n";
    std::wcout << "Para iniciar, digite o nome da janela: ";
    std::getline(std::wcin, windowName);



    /// LPCWSTR windowName = L"c";

   /// HWND hWnd = FindWindow(0, windowName.c_str());


   /// LPCWSTR gameone = L"c";

    HWND hWnd = FindWindow(0, windowName.c_str());

    GetWindowThreadProcessId(hWnd, &pid);

    if (IsProcessRunning(pid)) {

        std::wcout << "Agora digite o NOVO nome para a janela: ";
        std::getline(std::cin, newWindowName);
        std::wstring windowName = s2ws(newWindowName.c_str());
        LPCWSTR result = windowName.c_str();
        SetWindowText(hWnd, result);

        HWND hWnd = FindWindow(0, result);

        GetWindowThreadProcessId(hWnd, &pid);
        HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
        DWORD rf_client = GetModuleBase(L"DoNPatch.dll", pid);
        DWORD baseAddress = rf_client + 0x005642C;
        DWORD address = 0;
        ReadProcessMemory(pHandle, (void*)baseAddress, &address, sizeof(address), 0);
        address += 0x59C;
        ReadProcessMemory(pHandle, (void*)address, &address, sizeof(address), 0);
        address += 0x0;

        DWORD baseAddressCoordenadasReiTauron = rf_client + 0x005642C;
        DWORD addressCoordenadasReiTauron = 0;
        ReadProcessMemory(pHandle, (void*)baseAddressCoordenadasReiTauron, &addressCoordenadasReiTauron, sizeof(addressCoordenadasReiTauron), 0);
        addressCoordenadasReiTauron += 0x61C;
        ReadProcessMemory(pHandle, (void*)addressCoordenadasReiTauron, &addressCoordenadasReiTauron, sizeof(addressCoordenadasReiTauron), 0);
        addressCoordenadasReiTauron += 0x0;

        DWORD rf_client2 = GetModuleBase(L"SD Asgard.exe", pid);
        DWORD baseAddressNickname = rf_client2 + 0x01EDD54;
        DWORD addressNickname = 0;
        ReadProcessMemory(pHandle, (void*)baseAddressNickname, &addressNickname, sizeof(addressNickname), 0);
        addressNickname += 0x0;

        DWORD baseAddressPlayerX = rf_client2 + 0x01EDD58;
        DWORD addressPlayerX = 0;
        ReadProcessMemory(pHandle, (void*)baseAddressPlayerX, &addressPlayerX, sizeof(addressPlayerX), 0);
        addressPlayerX += 0x770;
        ReadProcessMemory(pHandle, (void*)addressPlayerX, &addressPlayerX, sizeof(addressPlayerX), 0);
        addressPlayerX += 0xC;
        ReadProcessMemory(pHandle, (void*)addressPlayerX, &addressPlayerX, sizeof(addressPlayerX), 0);
        addressPlayerX += 0x78;

        DWORD addressPlayerY = 0;
        ReadProcessMemory(pHandle, (void*)baseAddressPlayerX, &addressPlayerY, sizeof(addressPlayerY), 0);
        addressPlayerY += 0x770;
        ReadProcessMemory(pHandle, (void*)addressPlayerY, &addressPlayerY, sizeof(addressPlayerY), 0);
        addressPlayerY += 0xC;
        ReadProcessMemory(pHandle, (void*)addressPlayerY, &addressPlayerY, sizeof(addressPlayerY), 0);
        addressPlayerY += 0x7C;

        DWORD addressModoAtaque = rf_client2 + 0x20A9FC;
        DWORD addressPocaoHp = rf_client2 + 0x20AA04;
        DWORD addressRacaoPet = rf_client2 + 0x20AA00;
    }
    else {
        cout << "A janela digitada encontra-se fechada!" << endl;
        Sleep(2000);
        main();
    }

    while (true) {
        ReadProcessMemory(pHandle, (LPVOID)address, &target, 250, NULL);
        ReadProcessMemory(pHandle, (LPVOID)addressCoordenadasReiTauron, &coodenadasReiTauron, 250, NULL);
        ReadProcessMemory(pHandle, (LPVOID)addressNickname, &nickname, 250, NULL);
        ReadProcessMemory(pHandle, (LPVOID)addressPlayerX, &playerX, sizeof(addressPlayerX), 0);
        ReadProcessMemory(pHandle, (LPVOID)addressPlayerY, &playerY, sizeof(addressPlayerY), 0);
        ReadProcessMemory(pHandle, (LPVOID)addressModoAtaque, &modoAtaque, sizeof(addressModoAtaque), 0);
        ReadProcessMemory(pHandle, (LPVOID)addressPocaoHp, &pocaoHp, sizeof(addressPocaoHp), 0);
        ReadProcessMemory(pHandle, (LPVOID)addressRacaoPet, &racaoPet, sizeof(addressRacaoPet), 0);

        /// cout << target << endl;
        std::string s(target);
        std::string sNickname(nickname);
        std::string sCoordenada(coodenadasReiTauron);

        std::string MENSAGEM;
        s = ReplaceAll(s, std::string("Taurons!"), std::string(""));
        s = ReplaceAll(s, std::string("Faltam"), std::string(""));
        s = ReplaceAll(s, std::string(" "), std::string(""));
        ///cout << "\n\Letras coorde: " << sCoordenada.length() << "->" << sCoordenada << endl;



        playerX = (int)playerX;
        playerY = (int)playerY;
        modoAtaque = (int)modoAtaque;
        std::string modoAtaqueString;
        if (modoAtaque == 1) {
            modoAtaqueString = "FISICO";
        }
        else if (modoAtaque == 2) {
            modoAtaqueString = "MAGIA";
        }
        else if (modoAtaque == 3) {
            modoAtaqueString = "APENAS POTE";
        }
        else {
            modoAtaqueString = "DESATIVADO";
        }
        cout << "Modo Ataque: " << modoAtaque << endl;
        cout << "Modo Ataque: " << modoAtaqueString << endl;
        cout << "Auto HP: " << pocaoHp << "%" << endl;
        cout << "Alimentar PET: " << racaoPet << "%" << endl;



        int numeroTauros = atoi(std::string(s).c_str());
        // cout << "Faltam" << s << "Taurons!   - JANELA: " << newWindowName << endl;
        if (IsProcessRunning(pid)) {
            ///cout << "\n\Letras coorde: " << sNickname.length() << "->" << sNickname << endl;
            if (sNickname.length() >= 1) {

                if (numeroTauros > 2) {
                    /// node->Path((int)playerX, (int)playerY, 1169, 1699);
                    //// printf("Movendo de %dx %dy para %dx %dy", (int)playerX, (int)playerY, 1169, 1699);
                    /// node->Queue = 1;
                    /// playerX = node->Route[node->RouteActual].X;
                     ////playerY = node->Route[node->RouteActual].Y;

                    cout << "\n\nFaltam " << numeroTauros << " Taurons!  " << sCoordenada << "  \nJANELA: " << newWindowName << "\nPERSONAGEM: " << sNickname << "\n x:" << playerX << "  y:" << playerY << endl;
                    nasceu = false;
                    MENSAGEM = s;
                }
                if (numeroTauros <= 0) {
                    nasceu = true;

                    std::string xT(coodenadasReiTauron);
                    xT = ReplaceAll(xT, std::string("Rei vivo!"), std::string(""));
                    xT = ReplaceAll(xT, std::string("("), std::string(""));
                    xT = ReplaceAll(xT, std::string(")"), std::string(""));
                    vector<string> reiCoords;
                    reiCoords = split(xT, ' ');
                    int reiX = atoi(std::string(reiCoords[0]).c_str());
                    int reiY = atoi(std::string(reiCoords[1]).c_str());
                    cout << "\nO Rei Tauron pode ter nascido!\nJANELA: " << newWindowName << "\n\nCHAR: " << sNickname << "      \nx:" << playerX << "  y:" << playerY << "\n\nALVO: Rei Tauron      \nx:" << reiX << "  y:" << reiY << "\n\n" << endl;
                    int distRei = GetDistance(playerX, playerY, reiX, reiY);
                    cout << "Distancia do rei: " << distRei << " metros" << endl;

                    int modo = 2;
                    int hp = 30;
                    if (GetDistance(playerX, playerY, reiX, reiY) <= 12) {
                        BOOL alterarModoAtaque = WriteProcessMemory(pHandle, (LPVOID)addressModoAtaque, (LPVOID)&modo, sizeof((LPVOID)modo), NULL);
                        BOOL alterarPocao = WriteProcessMemory(pHandle, (LPVOID)addressPocaoHp, (LPVOID)&hp, sizeof((LPVOID)hp), NULL);
                        if (alterarModoAtaque && alterarPocao) {
                            cout << "O modo de ataque foi alterado para " << modoAtaqueString << " e pote hp para " << pocaoHp << "%" << "por estar perto do Rei" << endl;
                        }
                        else {
                            DWORD errCode = GetLastError();
                            cout << "Writing the memory failed!" << endl;
                            cout << "Error code: " << errCode << endl;
                        }
                    }
                    else {
                        int modo = 0;
                        int hp = 30;
                        BOOL alterarModoAtaque = WriteProcessMemory(pHandle, (LPVOID)addressModoAtaque, (LPVOID)&modo, sizeof((LPVOID)modo), NULL);
                        BOOL alterarPocao = WriteProcessMemory(pHandle, (LPVOID)addressPocaoHp, (LPVOID)&hp, sizeof((LPVOID)hp), NULL);
                        if (alterarModoAtaque && alterarPocao) {
                            cout << "O modo de ataque foi alterado para " << modoAtaqueString << " e pote hp para " << pocaoHp << "%" << "por estar longe do Rei" << endl;
                        }
                        else {
                            DWORD errCode = GetLastError();
                            cout << "Writing the memory failed!" << endl;
                            cout << "Error code: " << errCode << endl;
                        }
                    }

                    /// SendAction(playerX, playerY, node->Route[node->RouteActual].X, node->Route[node->RouteActual].Y, 4, 0);
                    /// node->Path(playerX, playerY, reiX, reiY);

                    /// ServerMessage("Caminhou para %dx %dy", Client.Position.X, Client.Position.Y);

                         ////cout << "\n\nAguardando o Rei Tauron morrer... \n" << endl;
                       ///  Beep(523, 3000); // 523 hertz (C5) por 500 milissegundos (0,5 segundos)
                     ///cin.get(); // espera tocar o som
                     ///      
                     ///
                    nasceu = false;
                    MENSAGEM = "Rei Tauron pode ter nascido no canal " + newWindowName + "," + sCoordenada;
                }

                if (newWindowName == "ODIN-1") {
                    std::ofstream ofs("ODIN-1.txt", std::ofstream::out);
                    ofs << MENSAGEM;
                    ofs.close();
                }
                else if (newWindowName == "ODIN-2") {
                    std::ofstream ofs("ODIN-2.txt", std::ofstream::out);
                    ofs << MENSAGEM;
                    ofs.close();
                }
                else if (newWindowName == "ODIN-3") {
                    std::ofstream ofs("ODIN-3.txt", std::ofstream::out);
                    ofs << MENSAGEM;
                    ofs.close();
                }
                else
                {

                }
                ////  cout << MENSAGEM << endl;

            }
            else {
                cout << "Conecte-se em algum personagem." << endl;
                ReadProcessMemory(pHandle, (LPVOID)address, &target, 250, NULL);
                ReadProcessMemory(pHandle, (LPVOID)addressCoordenadasReiTauron, &coodenadasReiTauron, 250, NULL);
                ReadProcessMemory(pHandle, (LPVOID)addressNickname, &nickname, 250, NULL);
                ReadProcessMemory(pHandle, (LPVOID)addressPlayerX, &playerX, sizeof(addressPlayerX), 0);
                ReadProcessMemory(pHandle, (LPVOID)addressPlayerY, &playerY, sizeof(addressPlayerY), 0);

            }


        }
        else {
            cout << "A janela digitada encontra-se fechada!" << endl;
            Sleep(2000);
            main();

        }
        Sleep(500);
        HANDLE hStdout;

        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        cls(hStdout);
    }
    system("Pause");
}