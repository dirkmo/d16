// this is how to read single bytes from stdin
// first: switch to raw terminal mode

struct termios old_attr;
void makeraw() {
    struct termios term_attr;
    tcgetattr(STDIN_FILENO, &old_attr);
    cfmakeraw(&term_attr);
    tcsetattr(STDIN_FILENO, TCSANOW, &term_attr);
}

void restoreTerm() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
}


int keypressed()
{
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

vector<uint8_t> vInputData;
bool threadRunning = true;
mutex mutexInput;

void read_input_thread() {
    while(threadRunning) {
        if( keypressed() ) {
            uint8_t c;
            c = getchar();
            if( c == 3 ) {
                threadRunning = false;
                restoreTerm();
                exit(0);
            }
            mutexInput.lock();
            vInputData.push_back(c);
            mutexInput.unlock();
            cout << c << " " << int(c) <<  flush;
        }
    }
}
