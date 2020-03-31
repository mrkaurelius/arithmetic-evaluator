#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MAX_STACK 2056
#define MAX_CHAR_IN 4096
#define PI_CONST 3.14159265358979323846
#define EU_CONST 2.71828182845904523536

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[01;33m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef struct {
  float item[MAX_STACK];
  int top;
} STACK;

// Stack functions

void initStack(STACK *s) { s->top = 0; }

int isEmpty(STACK *s) {
  if (!s->top) {
    return 1;
  } else {
    return 0;
  }
}

int isFull(STACK *s) {
  if (s->top == MAX_STACK) {
    return 1;
  } else {
    return 0;
  }
}

int push(STACK *s, float x) {
  if (isFull(s)) {
    return 0;
  }

  s->item[s->top] = x;
  s->top++;
  return 1;
}

int pop(STACK *s, float *x) {
  if (isEmpty(s)) {
    return 0;
  }

  s->top--;
  *x = s->item[s->top];
  return 1;
}

int peek(STACK *s, float *x) {
  if (isEmpty(s)) {
    return 0;
  }
  *x = s->item[s->top - 1];
  return 1;
}

void printStack(STACK *s) {
  int i;
  for (i = 0; i < s->top; i++) {
    printf("%c, ", (int)s->item[i]);
  }
  printf("\n");
}

// Arithmetic eval functions

/* Return 1 if ch1 has higher priority */
int isPrior(char ch1, char ch2) {
  char chList[7] = {'(', '|', '^', '*', '/', '+', '-'};
  char chPrio[7] = {5, 4, 3, 2, 2, 1, 1};
  short prv1 = -1, prv2 = -1;
  int i = 0;

  while (i < 7) {
    if (ch1 == chList[i]) {
      prv1 = chPrio[i];
    }
    if (ch2 == chList[i]) {
      prv2 = chPrio[i];
    }
    i++;
  }

  printf(ANSI_COLOR_RED);
  if (prv1 == -1) {
    printf("Unrecognized symbol '%c'\n", ch1);
  } else if (prv2 == -1) {
    printf("Unrecognized symbol '%c'\n", ch2);
  }
  printf(ANSI_COLOR_RESET);

  return (prv1 >= prv2);
}

/* Return 1 if the string contains the char  */
int isContain(char ch, char *in) {
  int i = 0, ilen = strlen(in);
  while (i < ilen) {
    if (in[i] == ch) {
      return 1;
    }
    i++;
  }
  return 0;
}

/* Free all the elements of string array */
void deleteExpression(char **strlist, int len) {
  int i;
  for (i = 0; i < len; i++) {
    free(strlist[i]);
  }
  free(strlist);
}

/* Return math function number, returns -1 if no function name matched */
int getFunctionNum(char *fname) {
  char *funcNames[8] = {"sin",    "cos",    "tan",    "cot",
                        "arctan", "arcsin", "arccos", "sqrt"};
  int i = 0;
  // Check for functions
  while (i < 8) {
    if (!strcasecmp(fname, funcNames[i])) {
      return i;
    }
    i++;
  }

  return -1;
}

int getConstant(char *fname, float *constVal, float formerAns) {
  char *constNames[3] = {"pi", "e", "m"};
  float constVals[3] = {PI_CONST, EU_CONST, formerAns};
  int i = 0;
  // Check for constants
  while (i < 3) {
    if (!strcasecmp(fname, constNames[i])) {
      *constVal = constVals[i];
      return 1;
    }
    i++;
  }

  // Not a constant
  return 0;
}

/* Takes a string, returns an array of strings,
 * e.g. "260+(4)" -> {"260","+","(","4",")"} */
char **strToArray(char *in, int slen, int *ilen, float formerAns) {
  char **str = (char **)malloc(sizeof(char *) * slen * 2);
  char tmp[MAX_CHAR_IN];
  float constVal;
  int i = 0, arrIndex = 0, t = 0, funcNum,
      textNumState = 0; // textNumState 1:text 2:num 0:don't care

  for (i = 0; i < slen; i++) {
    if (isdigit(in[i]) || in[i] == '.') {

      if (textNumState == 1) {
        // Switched from text state
        tmp[t++] = '\0';
        str[arrIndex] = (char *)malloc(sizeof(char) * (t + 16));

        funcNum = getFunctionNum(tmp);
        if (funcNum >= 0) {
          // Operand is a function
          sprintf(tmp, "%d", funcNum);
          strcpy(str[arrIndex++], tmp);

          // Put special math function indicator
          str[arrIndex] = (char *)malloc(sizeof(char) * (t + 16));
          strcpy(str[arrIndex++], "|");

        } else if (getConstant(tmp, &constVal, formerAns)) {
          // Operand is a constant
          sprintf(tmp, "%f", constVal);
          strcpy(str[arrIndex++], tmp);

        } else {
          // Neither constant nor function
          printf(ANSI_COLOR_RED);
          printf("Unknown function or constant: ");
          printf(ANSI_COLOR_RESET);
          printf("%s\n", tmp);
        }

        t = 0;
        tmp[t++] = in[i];
      } else {
        // Number state keeps
        tmp[t++] = in[i];
      }
      textNumState = 2;

    } else if (isalpha(in[i])) {

      if (textNumState == 2) {
        // Switched from num state
        tmp[t++] = '\0';
        str[arrIndex] = (char *)malloc(sizeof(char) * (t + 16));
        strcpy(str[arrIndex++], tmp);
        t = 0;

        tmp[t++] = in[i];
      } else {
        // Text state keeps
        tmp[t++] = in[i];
      }
      textNumState = 1;
    } else {

      if (textNumState == 1) {
        //  Transform text to function number
        tmp[t++] = '\0';
        str[arrIndex] = (char *)malloc(sizeof(char) * (t + 16));
        funcNum = getFunctionNum(tmp);
        if (funcNum >= 0) {
          // operand is a function
          sprintf(tmp, "%d", funcNum);
          strcpy(str[arrIndex++], tmp);

          // Put special math function indicator
          str[arrIndex] = (char *)malloc(sizeof(char) * (t + 16));
          strcpy(str[arrIndex++], "|");

        } else if (getConstant(tmp, &constVal, formerAns)) {
          // Operand is a constant
          sprintf(tmp, "%f", constVal);
          strcpy(str[arrIndex++], tmp);

        } else {
          // Neither constant nor function
          printf(ANSI_COLOR_RED);
          printf("Unknown function or constant: ");
          printf(ANSI_COLOR_RESET);
          printf("%s\n", tmp);
        }
      } else if (t) {
        tmp[t++] = '\0';
        str[arrIndex] = (char *)malloc(sizeof(char) * (t + 16));
        strcpy(str[arrIndex++], tmp);
      }

      str[arrIndex] = (char *)malloc(sizeof(char) * 16);
      str[arrIndex][0] = in[i];
      str[arrIndex][1] = '\0';
      arrIndex++;
      t = 0;
      textNumState = 0;
    }
  }
  if (t) {
    tmp[t] = '\0';
    str[arrIndex] = (char *)malloc(sizeof(char) * (t + 1));

    funcNum = getFunctionNum(tmp);
    if (funcNum >= 0) {
      // operand is a function
      sprintf(tmp, "%d", funcNum);
      strcpy(str[arrIndex++], tmp);

      // Put special math function indicator
      str[arrIndex] = (char *)malloc(sizeof(char) * (t + 16));
      strcpy(str[arrIndex++], "|");

    } else if (getConstant(tmp, &constVal, formerAns)) {
      // Operand is a constant
      sprintf(tmp, "%f", constVal);
      strcpy(str[arrIndex++], tmp);

    } else {
      // Neither constant nor function
      strcpy(str[arrIndex++], tmp);
    }
  }

  *ilen = arrIndex;
  // str = (char **)realloc(str, sizeof(char *) * arrIndex);
  return str;
}

char **infixToPostfix(char **in, int ilen, int *plen) {
  char **postf = (char **)malloc(sizeof(char *) * ilen);
  STACK *s = (STACK *)malloc(sizeof(STACK));
  int i, p = 0;
  float tmp;
  char op[MAX_CHAR_IN];
  initStack(s);

  for (i = 0; i < ilen; i++) {
    strcpy(op, in[i]);

    if (isdigit(op[0])) {
      // A digit
      postf[p] = (char *)malloc(sizeof(char) * strlen(op));
      strcpy(postf[p++], op);

    } else if (!strcmp(op, ")")) {
      peek(s, &tmp);
      while (!isEmpty(s) && (char)tmp != '(') {
        pop(s, &tmp);
        postf[p] = (char *)malloc(sizeof(char) * 16);
        postf[p][0] = tmp;
        postf[p++][1] = '\0';
        peek(s, &tmp);
      }
      pop(s, &tmp);
    } else if (!isEmpty(s)) {
      peek(s, &tmp);
      while (isPrior((char)tmp, op[0]) && !isEmpty(s) && (char)tmp != '(') {
        pop(s, &tmp);
        postf[p] = (char *)malloc(sizeof(char) * 16);
        postf[p][0] = tmp;
        postf[p++][1] = '\0';
        peek(s, &tmp);
      }
      push(s, (int)op[0]);
    } else {
      push(s, (int)op[0]);
    }
  }

  while (!isEmpty(s)) {
    pop(s, &tmp);
    postf[p] = (char *)malloc(sizeof(char) * 16);
    postf[p][0] = tmp;
    postf[p++][1] = '\0';
  }

  free(s);
  *plen = p;
  return postf;
}

float mathFunction(int func_num, float input) {
  float ans;

  switch (func_num) {
  case 0:
    // Sinus
    ans = sin(input);
    break;
  case 1:
    // Cosinus
    ans = cos(input);
    break;
  case 2:
    // Tangent
    ans = tan(input);
    break;
  case 3:
    // Cotangent
    ans = cos(input) / sin(input);
    break;
  case 4:
    // Arc-tangent
    ans = atan(input);
    break;
  case 5:
    // Arc-sinus
    ans = asin(input);
    break;
  case 6:
    // Arc-cosinus
    ans = acos(input);
    break;
  case 7:
    // Square root
    ans = sqrt(input);
    break;
  default:
    ans = 0;
    break;
  }

  return ans;
}

float perform(char op, float op1, float op2) {
  float ans;

  switch (op) {
  case '+':
    ans = op1 + op2;
    break;
  case '-':
    ans = op1 - op2;
    break;
  case '/':
    ans = op1 / op2;
    break;
  case '*':
    ans = op1 * op2;
    break;
  case '^':
    ans = pow(op1, op2);
    break;
  case '|':
    ans = mathFunction(op1, op2);
    break;
  default:
    ans = 0;
    break;
  }
  return ans;
}

/* Evaluate the operations, with using stack */
float evalPostfix(char **postf, int plen) {
  STACK *s = (STACK *)malloc(sizeof(STACK));
  int i;
  float op1, op2;

  for (i = 0; i < plen; i++) {
    if (isdigit(postf[i][0])) {
      push(s, atof(postf[i]));
    } else {
      pop(s, &op2);
      pop(s, &op1);
      push(s, perform(postf[i][0], op1, op2));
    }
  }
  pop(s, &op1);
  free(s);
  return op1;
}

/* Handles some minus operator cases, e.g."-4+1" */
char *preProcess(char *infix) {
  char *clean = (char *)malloc(sizeof(char) * MAX_CHAR_IN);
  int i = 0, c = 0;

  // Put zero to beginning, if starts with "-"
  if (infix[0] == '-') {
    clean[c++] = '0';
  }

  while (infix[i] != '\0') {
    // Put zero before negative leftsided numbers
    if (i > 0 && infix[i] == '-' && (infix[i - 1] == '(')) {
      clean[c++] = '0';
    }
    // Aceppts all chars but space
    if (infix[i] != ' ') {
      clean[c++] = infix[i];
    }
    i++;
  }
  clean[c] = '\0';

  return clean;
}

/* Returns 1 if the expression has balanced pharentheses*/
int validCheck(char *in, int ilen) {
  STACK *s = (STACK *)malloc(sizeof(STACK));
  float ch;
  int i;
  printf(ANSI_COLOR_RED);

  for (i = 0; i < ilen; i++) {
    if (in[i] == '(') {
      push(s, in[i]);
    } else {
      if (in[i] == ')') {
        if (!isEmpty(s)) {
          pop(s, &ch);
        } else {
          printf("Syntax Error: Unbalanced Pharentheses\n");
          free(s);
          return 0;
        }
      }
    }
  }
  if (!isEmpty(s)) {
    printf("Syntax Error: Unbalanced Pharentheses\n");
    free(s);
    return 0;
  }

  printf(ANSI_COLOR_RESET);
  free(s);
  return 1;
}

/* Evaluates and returns the answer. status will be set to -1 if any error
 * occurs */
float evalExpression(char *exp, int *status, float formerAns) {
  char **postf, **infix;
  int i, ilen, plen;
  float ans;
  *status = 1;
  // Preprocess
  strcpy(exp, preProcess(exp));
  // Validation check
  if (!validCheck(exp, strlen(exp))) {
    *status = -1;
    return 0;
  }

  infix = strToArray(exp, strlen(exp), &ilen, formerAns);
  /*for (i = 0; i < ilen; i++) {
    printf("%s, ", infix[i]);
  }
  printf("\n");*/

  postf = infixToPostfix(infix, ilen, &plen);
  /*
  for (i=0; i<plen; i++) {
    printf("len:%d :: ", (int)strlen(postf[i]));
    printf("%s, \n", postf[i]);
  }
  printf("\n");*/
  ans = evalPostfix(postf, plen);
  deleteExpression(infix, ilen);
  deleteExpression(postf, plen);
  return ans;
}

int main() {
  char in[MAX_CHAR_IN];
  int status;
  float ans, formerAns = 0;
  printf(ANSI_COLOR_RESET);
  printf("[Arithmetic Expression Evaluation]\n<Type 'q' to quit.>\n");

  do {
    printf(ANSI_COLOR_RESET);
    printf("E:");
    fgets(in, MAX_CHAR_IN, stdin);
    in[strlen(in) - 1] = '\0';

    ans = evalExpression(in, &status, formerAns);

    if (status > 0) {
      printf("Ans:>");
      printf(ANSI_COLOR_YELLOW);

      if (floor(ans) == (ans)) {
        printf("%d\n", (int)ans);
      } else {
        printf("%f\n", ans);
      }
      formerAns = ans;
    }
  } while (in[0] != 'q');

  return 0;
}
