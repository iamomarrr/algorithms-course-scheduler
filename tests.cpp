#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Algorithms.h"

long long testEnrollmentTotal(const std::vector<Course>& courses) {
    long long total = 0;

    for (std::size_t i = 0; i < courses.size(); ++i) {
        total += courses[i].enrollments;
    }

    return total;
}

void checkSameCourse(const Course& actual, const Course& expected) {
    CHECK(actual.id == expected.id);
    CHECK(actual.start == expected.start);
    CHECK(actual.finish == expected.finish);
    CHECK(actual.enrollments == expected.enrollments);
}

void checkSameList(
    const std::vector<Course>& actual,
    const std::vector<Course>& expected) {
    REQUIRE(actual.size() == expected.size());

    for (std::size_t i = 0; i < actual.size(); ++i) {
        checkSameCourse(actual[i], expected[i]);
    }
}

void checkReturnedCourses(
    const std::vector<Course>& selected,
    const std::vector<Course>& input) {
    for (std::size_t i = 0; i < selected.size(); ++i) {
        std::size_t matches = 0;

        for (std::size_t j = 0; j < input.size(); ++j) {
            if (selected[i].id == input[j].id) {
                ++matches;
                checkSameCourse(selected[i], input[j]);
            }
        }

        CHECK(matches == 1);

        for (std::size_t j = i + 1; j < selected.size(); ++j) {
            CHECK(selected[i].id != selected[j].id);
        }
    }
}

void checkSchedule(
    const std::vector<Course>& selected,
    const std::vector<Course>& input) {
    checkReturnedCourses(selected, input);

    for (std::size_t i = 0; i < selected.size(); ++i) {
        for (std::size_t j = i + 1; j < selected.size(); ++j) {
            CHECK(selected[i].finish <= selected[j].start);
        }
    }
}

void checkProblem(const std::vector<Course>& input) {
    std::vector<Course> original = input;
    std::vector<Course> sorted = input;

    mergeSort(sorted);

    REQUIRE(sorted.size() == input.size());
    checkReturnedCourses(sorted, input);

    for (std::size_t i = 1; i < sorted.size(); ++i) {
        if (sorted[i - 1].finish == sorted[i].finish) {
            if (sorted[i - 1].start == sorted[i].start) {
                CHECK(sorted[i - 1].id < sorted[i].id);
            } else {
                CHECK(sorted[i - 1].start < sorted[i].start);
            }
        } else {
            CHECK(sorted[i - 1].finish < sorted[i].finish);
        }
    }

    std::vector<Course> sortedBeforeSelection = sorted;

    std::vector<Course> count = selectCourses(sorted);
    std::vector<Course> weighted = selectCoursesDP(sorted);

    std::vector<std::vector<Course>> rooms =
        allocateClassrooms(input);

    checkSameList(sorted, sortedBeforeSelection);
    checkSameList(input, original);

    checkSchedule(count, input);
    checkSchedule(weighted, input);

    std::vector<Course> assigned;

    for (std::size_t room = 0; room < rooms.size(); ++room) {
        CHECK(rooms[room].size() > 0);
        checkSchedule(rooms[room], input);

        for (std::size_t i = 0; i < rooms[room].size(); ++i) {
            assigned.push_back(rooms[room][i]);
        }
    }

    CHECK(assigned.size() == input.size());
    checkReturnedCourses(assigned, input);

    if (input.size() <= 5) {
        CHECK(count.size() == optimalCountExhaustive(input));

        CHECK(
            testEnrollmentTotal(weighted) ==
            optimalEnrollmentsExhaustive(input)
        );

        CHECK(rooms.size() == minClassroomsExhaustive(input));

        checkSameList(input, original);
    }
}

TEST_CASE("Empty input") {
    std::vector<Course> courses;

    checkProblem(courses);

    CHECK(selectCourses(courses).size() == 0);
    CHECK(selectCoursesDP(courses).size() == 0);
    CHECK(allocateClassrooms(courses).size() == 0);
}

TEST_CASE("Assignment sample distinguishes count from enrollments") {
    std::vector<Course> courses = {
        {"Algorithms", 540, 600, 4},
        {"Databases", 600, 660, 4},
        {"Operating_Systems", 540, 660, 11}
    };

    checkProblem(courses);
    mergeSort(courses);

    std::vector<Course> count = selectCourses(courses);

    REQUIRE(count.size() == 2);
    CHECK(count[0].id == "Algorithms");
    CHECK(count[1].id == "Databases");
    CHECK(testEnrollmentTotal(count) == 8);

    std::vector<Course> weighted = selectCoursesDP(courses);

    REQUIRE(weighted.size() == 1);
    CHECK(weighted[0].id == "Operating_Systems");
    CHECK(testEnrollmentTotal(weighted) == 11);
}

TEST_CASE("Assignment ties use identifiers and skip equal DP alternatives") {
    std::vector<Course> courses = {
        {"Databases", 540, 600, 5},
        {"Algorithms", 540, 600, 5},
        {"Operating_Systems", 600, 660, 2}
    };

    checkProblem(courses);
    mergeSort(courses);

    std::vector<Course> weighted = selectCoursesDP(courses);

    REQUIRE(weighted.size() == 2);
    CHECK(weighted[0].id == "Algorithms");
    CHECK(weighted[1].id == "Operating_Systems");
    CHECK(testEnrollmentTotal(weighted) == 7);
}

TEST_CASE("One course and totals greater than a 32 bit signed integer") {
    std::vector<Course> courses = {
        {"A", 0, 10, 3000000000LL}
    };

    checkProblem(courses);

    courses.push_back({"B", 10, 20, 3000000000LL});

    checkProblem(courses);
    mergeSort(courses);

    CHECK(
        testEnrollmentTotal(selectCoursesDP(courses)) ==
        6000000000LL
    );
}

TEST_CASE("All courses overlap") {
    checkProblem({
        {"A", 0, 30, 5},
        {"B", 10, 40, 20},
        {"C", 20, 50, 9}
    });
}

TEST_CASE("An overlapping immediate predecessor must not hide an earlier match") {
    std::vector<Course> courses = {
        {"A", 0, 3, 4},
        {"B", 1, 5, 5},
        {"C", 3, 6, 10}
    };

    checkProblem(courses);
    mergeSort(courses);

    std::vector<Course> weighted = selectCoursesDP(courses);

    REQUIRE(weighted.size() == 2);
    CHECK(weighted[0].id == "A");
    CHECK(weighted[1].id == "C");
    CHECK(testEnrollmentTotal(weighted) == 14);
}

TEST_CASE("Earliest start and shortest duration are not valid count rules") {
    checkProblem({
        {"Long", 0, 10, 1},
        {"A", 1, 3, 1},
        {"B", 3, 5, 1},
        {"C", 5, 7, 1}
    });

    checkProblem({
        {"A", 0, 4, 1},
        {"B", 4, 8, 1},
        {"Short", 3, 5, 1}
    });
}

TEST_CASE("Unsorted input and equal starts or finishes") {
    checkProblem({
        {"Z", 20, 40, 8},
        {"B", 0, 20, 4},
        {"A", 0, 20, 4},
        {"Early", 0, 10, 3},
        {"Late", 10, 20, 3}
    });
}

TEST_CASE("Rooms choose the earliest finishing available room") {
    std::vector<Course> courses = {
        {"P", 0, 10, 1},
        {"Q", 1, 5, 1},
        {"R", 10, 12, 1}
    };

    checkProblem(courses);

    std::vector<std::vector<Course>> rooms =
        allocateClassrooms(courses);

    REQUIRE(rooms.size() == 2);
    REQUIRE(rooms[0].size() == 1);
    REQUIRE(rooms[1].size() == 2);

    CHECK(rooms[0][0].id == "P");
    CHECK(rooms[1][0].id == "Q");
    CHECK(rooms[1][1].id == "R");
}

TEST_CASE("Rooms choose the smaller number when finishing times are equal") {
    std::vector<Course> courses = {
        {"P", 0, 5, 1},
        {"Q", 1, 5, 1},
        {"R", 5, 7, 1}
    };

    checkProblem(courses);

    std::vector<std::vector<Course>> rooms =
        allocateClassrooms(courses);

    REQUIRE(rooms.size() == 2);
    REQUIRE(rooms[0].size() == 2);

    CHECK(rooms[0][1].id == "R");
}

TEST_CASE("Equal enrollment totals do not favor selecting more courses") {
    std::vector<Course> courses = {
        {"A", 0, 10, 10},
        {"B", 0, 5, 5},
        {"C", 5, 10, 5}
    };

    checkProblem(courses);
    mergeSort(courses);

    std::vector<Course> weighted = selectCoursesDP(courses);

    REQUIRE(weighted.size() == 1);
    CHECK(weighted[0].id == "A");
}

TEST_CASE("120 reproducible generated cases compare all three optima") {
    std::string names[] = {"A", "B", "C", "D", "E"};

    for (int number = 0; number < 120; ++number) {
        // Display the case number if an assertion fails.
        CAPTURE(number);

        std::vector<Course> courses;
        int n = number % 6;

        for (int i = 0; i < n; ++i) {
            int start = ((number * 7 + i * 11) % 8) * 10;
            int finish = start + 10 * (1 + (number + i * 3) % 4);

            long long enrollments =
                1 + (number * 13 + i * 17) % 30;

            courses.push_back({
                names[i], start, finish, enrollments
            });
        }

        checkProblem(courses);
    }
}

TEST_CASE("More than five courses checks feasibility without exhaustive search") {
    std::vector<Course> courses = {
        {"H", 45, 55, 8},
        {"G", 40, 50, 7},
        {"F", 30, 40, 4},
        {"E", 20, 40, 12},
        {"D", 20, 30, 4},
        {"C", 10, 20, 4},
        {"B", 0, 20, 9},
        {"A", 0, 10, 4}
    };

    checkProblem(courses);

    CHECK(allocateClassrooms(courses).size() == 2);

    mergeSort(courses);

    CHECK(selectCourses(courses).size() == 5);
    CHECK(testEnrollmentTotal(selectCoursesDP(courses)) == 29);
}