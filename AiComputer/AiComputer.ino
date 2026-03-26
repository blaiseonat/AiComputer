#include <Arduino.h>
#include <string.h>
#include <ctype.h>

char input[64];
byte inputIndex = 0;

bool plotting = false;
byte plotMode = 0; // 0 = none, 1 = sine, 2 = cosine, 3 = ramp
unsigned long lastPlot = 0;
float tValue = 0.0;

void toLowerInPlace(char *s) {
  while (*s) {
    *s = tolower(*s);
    s++;
  }
}

bool containsWord(const char *text, const char *word) {
  char buf[64];
  strncpy(buf, text, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';
  toLowerInPlace(buf);

  char wbuf[32];
  strncpy(wbuf, word, sizeof(wbuf) - 1);
  wbuf[sizeof(wbuf) - 1] = '\0';
  toLowerInPlace(wbuf);

  char wrapped[68];
  snprintf(wrapped, sizeof(wrapped), " %s ", buf);

  char wwrapped[36];
  snprintf(wwrapped, sizeof(wwrapped), " %s ", wbuf);

  return strstr(wrapped, wwrapped) != NULL;
}

void fakeWebSearchIntro() {
  Serial.println(F("Pretending to search the web..."));
  delay(150);
  Serial.println(F("Thinking..."));
  delay(150);
}

void answerFact(const char *q) {
  char buf[64];
  strncpy(buf, q, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';
  toLowerInPlace(buf);

  if (strstr(buf, "capital of japan")) {
    Serial.println(F("The capital of Japan is Tokyo."));
  } else if (strstr(buf, "capital of france")) {
    Serial.println(F("The capital of France is Paris."));
  } else if (strstr(buf, "capital of germany")) {
    Serial.println(F("The capital of Germany is Berlin."));
  } else if (strstr(buf, "albert einstein")) {
    Serial.println(F("Albert Einstein was a physicist known for the theory of relativity."));
  } else if (strstr(buf, "isaac newton")) {
    Serial.println(F("Isaac Newton was a scientist known for the laws of motion and gravity."));
  } else if (strstr(buf, "speed of light")) {
    Serial.println(F("The speed of light in vacuum is about 299,792,458 meters per second."));
  } else if (strstr(buf, "who are you") || strstr(buf, "what are you")) {
    Serial.println(F("I am a tiny AI-style mini computer running on a microcontroller, talking through Serial."));
  } else {
    Serial.println(F("I don't have a fact stored for that yet."));
  }
}

void answerMathExpression(char *expr) {
  char buf[32];
  byte j = 0;
  for (byte i = 0; expr[i] != '\0' && j < sizeof(buf) - 1; i++) {
    if (expr[i] != ' ') {
      buf[j++] = expr[i];
    }
  }
  buf[j] = '\0';

  char *p;
  long a, b;

  if ((p = strchr(buf, '+'))) {
    *p = '\0';
    a = atol(buf);
    b = atol(p + 1);
    Serial.print(F("Result: "));
    Serial.println(a + b);
  } else if ((p = strchr(buf, '*'))) {
    *p = '\0';
    a = atol(buf);
    b = atol(p + 1);
    Serial.print(F("Result: "));
    Serial.println(a * b);
  } else if ((p = strchr(buf, '-'))) {
    *p = '\0';
    a = atol(buf);
    b = atol(p + 1);
    Serial.print(F("Result: "));
    Serial.println(a - b);
  } else if ((p = strchr(buf, '/'))) {
    *p = '\0';
    a = atol(buf);
    b = atol(p + 1);
    if (b == 0) {
      Serial.println(F("Error: division by zero."));
    } else {
      Serial.print(F("Result: "));
      Serial.println(a / b);
    }
  } else {
    Serial.println(F("I couldn't understand that math expression."));
  }
}

void startPlot(byte mode) {
  plotting = true;
  plotMode = mode;
  tValue = 0.0;
  lastPlot = millis();
  Serial.println(F("Plotting started. Open Serial Plotter."));
}

void stopPlot() {
  plotting = false;
  plotMode = 0;
  Serial.println(F("Plotting stopped."));
}

void answerHelp() {
  Serial.println(F("AI Mini Computer – Commands:"));
  Serial.println(F("  help           - show this help"));
  Serial.println(F("  hello / hi     - greeting"));
  Serial.println(F("  time           - show uptime"));
  Serial.println(F("  add X Y        - add two numbers"));
  Serial.println(F("  sub X Y        - subtract two numbers"));
  Serial.println(F("  what is 5+3    - simple math"));
  Serial.println(F("  ask ...        - simple fact question"));
  Serial.println(F("  plot sine      - plot sine wave"));
  Serial.println(F("  plot cosine    - plot cosine wave"));
  Serial.println(F("  plot ramp      - plot ramp"));
  Serial.println(F("  stop           - stop plotting"));
}

void processCommand(char *cmd) {
  while (*cmd == ' ') cmd++;
  int len = strlen(cmd);
  while (len > 0 && (cmd[len - 1] == ' ' || cmd[len - 1] == '\r')) {
    cmd[len - 1] = '\0';
    len--;
  }
  if (len == 0) return;

  char lower[64];
  strncpy(lower, cmd, sizeof(lower) - 1);
  lower[sizeof(lower) - 1] = '\0';
  toLowerInPlace(lower);

  if (strcmp(lower, "help") == 0) {
    answerHelp();
    return;
  }

  if (containsWord(lower, "hello") || containsWord(lower, "hi") || containsWord(lower, "hey")) {
    Serial.println(F("Hello! I'm your mini AI computer. What do you want to do?"));
    return;
  }

  if (strcmp(lower, "time") == 0 || strcmp(lower, "uptime") == 0) {
    Serial.print(F("Uptime (ms): "));
    Serial.println(millis());
    return;
  }

  if (strncmp(lower, "echo ", 5) == 0) {
    Serial.print(F("You said: "));
    Serial.println(cmd + 5);
    return;
  }

  if (strncmp(lower, "add ", 4) == 0) {
    char *p = lower + 4;
    char *space = strchr(p, ' ');
    if (space) {
      *space = '\0';
      long a = atol(p);
      long b = atol(space + 1);
      Serial.print(F("Result: "));
      Serial.println(a + b);
    } else {
      Serial.println(F("Usage: add X Y"));
    }
    return;
  }

  if (strncmp(lower, "sub ", 4) == 0) {
    char *p = lower + 4;
    char *space = strchr(p, ' ');
    if (space) {
      *space = '\0';
      long a = atol(p);
      long b = atol(space + 1);
      Serial.print(F("Result: "));
      Serial.println(a - b);
    } else {
      Serial.println(F("Usage: sub X Y"));
    }
    return;
  }

  if (strncmp(lower, "what is", 7) == 0) {
    char *pos = strstr(lower, "what is");
    char *expr = cmd + (pos - lower) + 7;
    while (*expr == ' ') expr++;
    if (*expr) {
      answerMathExpression(expr);
    } else {
      Serial.println(F("You said 'what is' but gave no expression."));
    }
    return;
  }

  if (strncmp(lower, "ask ", 4) == 0) {
    fakeWebSearchIntro();
    answerFact(cmd + 4);
    return;
  }

  if (strncmp(lower, "who is", 6) == 0 || strncmp(lower, "what is", 7) == 0 || strncmp(lower, "where is", 8) == 0) {
    fakeWebSearchIntro();
    answerFact(cmd);
    return;
  }

  if (strncmp(lower, "plot", 4) == 0) {
    if (strstr(lower, "sine")) startPlot(1);
    else if (strstr(lower, "cosine")) startPlot(2);
    else if (strstr(lower, "ramp")) startPlot(3);
    else startPlot(1);
    return;
  }

  if (strcmp(lower, "stop") == 0) {
    stopPlot();
    return;
  }

  // YOUR CUSTOM "CLANKER" RESPONSE — FIXED
  if (containsWord(lower, "clanker")) {
    Serial.println(F("That's mean! I'm not a clanker, I'm a smart AI."));
    return;
  }

  if (containsWord(lower, "you")) {
    Serial.println(F("I am a small AI-style program running on a microcontroller."));
  } else if (containsWord(lower, "bored")) {
    Serial.println(F("You can try math, facts, or 'plot sine' to see a graph."));
  } else if (containsWord(lower, "joke")) {
    Serial.println(F("Here's a tiny joke: Why did the bit flip? It found a better state."));
  } else {
    Serial.print(F("I don't fully understand: '"));
    Serial.print(cmd);
    Serial.println(F("'. Type 'help' to see what I can do."));
  }
}

void setup() {
  Serial.begin(9600);
  delay(300);
  Serial.println(F("================================"));
  Serial.println(F("  AI MINI COMPUTER – SERIAL"));
  Serial.println(F("================================"));
  Serial.println(F("Boot complete. Type 'help'."));
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      input[inputIndex] = '\0';
      processCommand(input);
      inputIndex = 0;
    } else {
      if (inputIndex < sizeof(input) - 1) {
        input[inputIndex++] = c;
      }
    }
  }

  if (plotting && millis() - lastPlot > 30) {
    lastPlot = millis();
    float y = 0.0;
    if (plotMode == 1) y = sin(tValue);
    else if (plotMode == 2) y = cos(tValue);
    else if (plotMode == 3) y = tValue;

    Serial.println(y);
    tValue += 0.1;
  }
}
