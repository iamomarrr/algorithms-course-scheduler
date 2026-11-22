#include "Algorithms.h"
#include <iostream>

void printCourse(const Course& course) {
    std::cout << course.id << ' '
              << course.start << ' '
              << course.finish << ' '
              << course.enrollments << '\n';
}

void printSelection(
    const std::string& heading,
    const std::vector<Course>& selected) {
    long long total = 0;

    for (std::size_t i = 0; i < selected.size(); ++i) {
        total += selected[i].enrollments;
    }

    std::cout << heading << '\n';
    std::cout << selected.size() << ' ' << total << '\n';

    for (std::size_t i = 0; i < selected.size(); ++i) {
        printCourse(selected[i]);
    }
}

int main() {
    std::size_t n;
    std::cin >> n;

    std::vector<Course> courses(n);

    for (std::size_t i = 0; i < n; ++i) {
        std::cin >> courses[i].id
                 >> courses[i].start
                 >> courses[i].finish
                 >> courses[i].enrollments;
    }

    std::vector<Course> sorted = courses;
    mergeSort(sorted);

    std::vector<Course> count = selectCourses(sorted);

    std::vector<std::vector<Course>> rooms =
        allocateClassrooms(courses);

    std::vector<Course> enrollments =
        selectCoursesDP(sorted);

    printSelection("COUNT", count);

    std::cout << "ROOMS\n";
    std::cout << rooms.size() << '\n';

    for (std::size_t room = 0; room < rooms.size(); ++room) {
        for (std::size_t i = 0; i < rooms[room].size(); ++i) {
            std::cout << room + 1 << ' ';
            printCourse(rooms[room][i]);
        }
    }

    printSelection("ENROLLMENTS", enrollments);

    return 0;
}