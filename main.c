#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EVENTS 100

typedef enum {
    CMD_NONE,
    CMD_NEXT,
    CMD_PREV,
    CMD_EXIT,
    CMD_ADD,
    CMD_DELETE,
    CMD_EDIT
} CommandType;

CommandType getCommandType(char command) {
    switch (command) {
        case 'n': return CMD_NEXT;
        case 'p': return CMD_PREV;
        case 'x': return CMD_EXIT;
        case 'a': return CMD_ADD;
        case 'd': return CMD_DELETE;
        case 'e': return CMD_EDIT;
        default:  return CMD_NONE;
    }
}

typedef struct {
    int year, month, day;
    char event[100];
} CalendarEvent;

CalendarEvent events[MAX_EVENTS];
int eventCount = 0;
int currentYear, currentMonth;

void addEvent(int day, int month, int year) {
    if (eventCount >= MAX_EVENTS) {
        printf("이벤트 저장 공간이 부족합니다.\n");
        return;
    }

    events[eventCount].year = year;
    events[eventCount].month = month;
    events[eventCount].day = day;
    printf("이벤트 내용을 입력하세요: ");
    scanf(" %[^\n]", events[eventCount].event);
    eventCount++;
}

int listEvents(int day, int month, int year) {
    int count = 0;
    printf("날짜 %04d/%02d/%02d의 이벤트 목록:\n", year, month, day);
    for (int i = 0; i < eventCount; i++) {
        if (events[i].day == day && events[i].month == month && events[i].year == year) {
            printf("[%d] %s\n", count+1, events[i].event);
            count++;
        }
    }
    return count;
}

void deleteEvent(int day, int month, int year) {
    int count = listEvents(day, month, year);
    if (count == 0) {
        printf("해당 날짜에 이벤트가 없습니다.\n");
        return;
    }

    int choice;
    if (count == 1) {
        choice = 1;
    } else {
        printf("삭제할 이벤트 번호를 선택하세요: ");
        scanf("%d", &choice);
        if (choice < 1 || choice > count) {
            printf("잘못된 선택입니다.\n");
            return;
        }
    }

    // 선택된 이벤트를 삭제
    for (int i = 0; i < eventCount; i++) {
        if (events[i].day == day && events[i].month == month && events[i].year == year) {
            if (--choice == 0) {
                for (int j = i; j < eventCount - 1; j++) {
                    events[j] = events[j + 1];
                }
                eventCount--;
                printf("이벤트가 삭제되었습니다.\n");
                return;
            }
        }
    }
}

void editEvent(int day, int month, int year) {
    int count = listEvents(day, month, year);
    if (count == 0) {
        printf("해당 날짜에 이벤트가 없습니다.\n");
        return;
    }

    int choice;
    if (count == 1) {
        choice = 1;
    } else {
        printf("수정할 이벤트 번호를 선택하세요: ");
        scanf("%d", &choice);
        if (choice < 1 || choice > count) {
            printf("잘못된 선택입니다.\n");
            return;
        }
    }

    // 선택된 이벤트를 수정
    for (int i = 0; i < eventCount; i++) {
        if (events[i].day == day && events[i].month == month && events[i].year == year) {
            if (--choice == 0) {
                printf("새로운 이벤트 내용을 입력하세요: ");
                scanf(" %[^\n]", events[i].event);
                printf("이벤트가 수정되었습니다.\n");
                return;
            }
        }
    }
}

void saveEventsToFile() {
    FILE *file = fopen("events.txt", "w");
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return;
    }
    for (int i = 0; i < eventCount; i++) {
        // fprintf(file, "%02d/%02d/%04d: %s\n", events[i].day, events[i].month, events[i].year, events[i].event);
      fprintf(file, "%04d/%02d/%02d: %s\n", events[i].year, events[i].month, events[i].day, events[i].event);

    }
    fclose(file);
}

void loadEventsFromFile() {
    FILE *file = fopen("events.txt", "r");
    if (file == NULL) {
        printf("이벤트 파일이 없습니다.\n");
        return;
    }
    while (fscanf(file, "%d/%d/%d: %[^\n]", 
      &events[eventCount].year, &events[eventCount].month, &events[eventCount].day, events[eventCount].event) == 4) {
        eventCount++;
        if (eventCount >= MAX_EVENTS) {
            printf("이벤트 저장 공간이 부족합니다.\n");
            break;
        }
    }
    fclose(file);
}

void changeMonth(int *year, int *month, int increment) {
    *month += increment;
    if (*month > 12) {
        *month = 1;
        *year += 1;
    } else if (*month < 1) {
        *month = 12;
        *year -= 1;
    }
}

int isLeapYear(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int getDaysInMonth(int month, int year) {
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return daysInMonth[month - 1];
}

int getFirstDayOfWeek(int month, int year) {
    struct tm timeStruct = {0};
    timeStruct.tm_year = year - 1900;
    timeStruct.tm_mon = month - 1;
    timeStruct.tm_mday = 1;
    mktime(&timeStruct);
    return timeStruct.tm_wday; // 0: Sunday, 1: Monday, ..., 6: Saturday
}

int hasEvent(int day, int month, int year) {
    for (int i = 0; i < eventCount; i++) {
        if (events[i].day == day && events[i].month == month && events[i].year == year) {
            return 1; // 이벤트가 있음
        }
    }
    return 0; // 이벤트가 없음
}

void printCalendar(int year, int month) {
    int days = getDaysInMonth(month, year);
    int startDay = getFirstDayOfWeek(month, year);
    printf("%d년 %d월\n", year, month);
    printf("일 월 화 수 목 금 토\n");

    // 시작 요일까지 공백으로 채우기
    for (int i = 0; i < startDay; i++) {
        printf("   ");
    }

    for (int day = 1; day <= days; day++) {
        if (hasEvent(day, month, year)) {
            printf("%2d*", day);
        } else {
            printf("%2d ", day);
        }
        if ((startDay + day) % 7 == 0) {
            printf("\n");
        }
    }
    printf("\n\n");
}


void printEventsForMonth(int year, int month) {
    int found = 0;
    printf("%d년 %d월의 이벤트:\n", year, month);
    for (int i = 0; i < eventCount; i++) {
        if (events[i].year == year && events[i].month == month) {
            // printf("%02d/%02d/%04d: %s\n", events[i].day, events[i].month, events[i].year, events[i].event);
            printf("%04d/%02d/%02d: %s\n", events[i].year, events[i].month, events[i].day, events[i].event);

            found = 1;
        }
    }
  printf("--------------------------\n");
    if (!found) {
        printf("이번 달에는 이벤트가 없습니다.\n");
    }
}

int main() {
    loadEventsFromFile();

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int year = tm.tm_year + 1900;
    int month = tm.tm_mon + 1;

    currentYear = year;
    currentMonth = month;

    while (1) {
      printCalendar(currentYear, currentMonth);
      printEventsForMonth(currentYear, currentMonth);
      char commandChar;
      CommandType command;
      printf("명령을 입력하세요 (n: Next, p: Prev, x: Exit, a: Add, d: Delete, e: Edit): ");
      scanf(" %c", &commandChar);
      command = getCommandType(commandChar);
      int day;
      switch (command) {
          case CMD_NEXT:
              changeMonth(&currentYear, &currentMonth, 1);
              break;
          case CMD_PREV:
              changeMonth(&currentYear, &currentMonth, -1);
              break;
          case CMD_ADD:
              printf("이벤트를 추가할 날짜 (일)을 입력하세요: ");
              scanf("%d", &day);
              addEvent(day, currentMonth, currentYear);
              break;
          case CMD_DELETE:
              printf("삭제할 이벤트의 날짜 (일)을 입력하세요: ");
              scanf("%d", &day);
              deleteEvent(day, currentMonth, currentYear);
              break;
          case CMD_EDIT:
              printf("수정할 이벤트의 날짜 (일)을 입력하세요: ");
              scanf("%d", &day);
              editEvent(day, currentMonth, currentYear);
              break;
          case CMD_EXIT:
              saveEventsToFile();
              return 0;
          case CMD_NONE:
          default:
              printf("잘못된 명령입니다.\n");
      }
      printf("\n");
    }
    saveEventsToFile();
    return 0;
}
