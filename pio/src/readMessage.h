int getMessageLength() {
    if (false) {
        return 4;
    }

    int old = 0;

    // Reset input buffer
    customSerial.flush();
    delay(100);

    // Get new input buffer size
    int new_size = customSerial.available();

    if (new_size > 0) {
        // Wait for complete data messages
        while (old < new_size) {
            delay(100);
            old = new_size;
            new_size = customSerial.available();
        }

        old = 0;
        new_size = 0;
        customSerial.flush();
    }

    // Wait until data is received
    while (old == new_size) {
        delay(100);
        old = new_size;
        new_size = customSerial.available();
    }

    // Wait for complete data messages
    while (old < new_size) {
        delay(100);
        old = new_size;
        new_size = customSerial.available();
    }

    return new_size;
}

int getMaximumMessageLength() {
    int max = 0;
    for (int i = 0; i < 5; i++) {
        int len = getMessageLength();
        if (len > max) {
            max = len;
        }
    }

    return max;
}