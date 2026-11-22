#include "Algorithms.h"

// byStart selects the ordering required for classroom allocation.
bool courseBefore(const Course& a, const Course& b, bool byStart) {
    if (byStart) {
        if (a.start != b.start) return a.start < b.start;
        if (a.finish != b.finish) return a.finish < b.finish;
    } else {
        if (a.finish != b.finish) return a.finish < b.finish;
        if (a.start != b.start) return a.start < b.start;
    }

    return a.id < b.id;
}

// Sort the half-open range [left, right).
void mergeSortRange(std::vector<Course>& courses,
                    std::vector<Course>& temporary,
                    std::size_t left,
                    std::size_t right,
                    bool byStart) {
    if (right - left <= 1) return;

    std::size_t middle = left + (right - left) / 2;

    mergeSortRange(courses, temporary, left, middle, byStart);
    mergeSortRange(courses, temporary, middle, right, byStart);

    std::size_t i = left;
    std::size_t j = middle;
    std::size_t k = left;

    while (i < middle && j < right) {
        if (courseBefore(courses[j], courses[i], byStart)) {
            temporary[k] = courses[j];
            ++j;
        } else {
            temporary[k] = courses[i];
            ++i;
        }

        ++k;
    }

    while (i < middle) {
        temporary[k] = courses[i];
        ++i;
        ++k;
    }

    while (j < right) {
        temporary[k] = courses[j];
        ++j;
        ++k;
    }

    for (std::size_t position = left; position < right; ++position) {
        courses[position] = temporary[position];
    }
}

void sortCourses(std::vector<Course>& courses, bool byStart) {
    std::vector<Course> temporary(courses.size());

    mergeSortRange(
        courses, temporary, 0, courses.size(), byStart
    );
}

void mergeSort(std::vector<Course>& courses) {
    sortCourses(courses, false);
}

std::vector<Course> selectCourses(
    const std::vector<Course>& sortedCourses) {
    std::vector<Course> selected;

    for (std::size_t i = 0; i < sortedCourses.size(); ++i) {
        if (selected.size() == 0 ||
            selected[selected.size() - 1].finish <=
                sortedCourses[i].start) {
            selected.push_back(sortedCourses[i]);
        }
    }

    return selected;
}

std::vector<std::vector<Course>> allocateClassrooms(
    const std::vector<Course>& courses) {
    std::vector<Course> sorted = courses;
    sortCourses(sorted, true);

    std::vector<std::vector<Course>> rooms;

    for (std::size_t i = 0; i < sorted.size(); ++i) {
        std::size_t bestRoom = rooms.size();

        for (std::size_t room = 0; room < rooms.size(); ++room) {
            int finish =
                rooms[room][rooms[room].size() - 1].finish;

            if (finish <= sorted[i].start) {
                if (bestRoom == rooms.size() ||
                    finish <
                        rooms[bestRoom][rooms[bestRoom].size() - 1].finish) {
                    bestRoom = room;
                }
            }
        }

        // Equal finishes keep the smaller room number because
        // rooms are scanned in increasing number order.
        if (bestRoom == rooms.size()) {
            rooms.push_back(std::vector<Course>());
        }

        rooms[bestRoom].push_back(sorted[i]);
    }

    return rooms;
}

std::vector<Course> selectCoursesDP(
    const std::vector<Course>& sortedCourses) {
    std::size_t n = sortedCourses.size();

    // Store p(j) + 1: zero means no compatible predecessor.
    // This also gives the length of the compatible DP prefix.
    std::vector<std::size_t> previous(n, 0);

    for (std::size_t j = 0; j < n; ++j) {
        std::size_t i = j;

        while (i > 0) {
            --i;

            if (sortedCourses[i].finish <= sortedCourses[j].start) {
                previous[j] = i + 1;
                break;
            }
        }
    }

    // dp[k] is the best total using the first k courses.
    std::vector<long long> dp(n + 1, 0);

    for (std::size_t j = 0; j < n; ++j) {
        long long include =
            sortedCourses[j].enrollments + dp[previous[j]];

        long long exclude = dp[j];

        dp[j + 1] = exclude;

        if (include > exclude) {
            dp[j + 1] = include;
        }
    }

    std::vector<Course> backwards;
    std::size_t k = n;

    while (k > 0) {
        std::size_t j = k - 1;

        long long include =
            sortedCourses[j].enrollments + dp[previous[j]];

        if (include > dp[k - 1]) {
            backwards.push_back(sortedCourses[j]);
            k = previous[j];
        } else {
            // Skip the current course when the totals are equal.
            --k;
        }
    }

    std::vector<Course> selected;

    for (std::size_t i = backwards.size(); i > 0; --i) {
        selected.push_back(backwards[i - 1]);
    }

    return selected;
}

// The exhaustive checkers do not call sorting, greedy selection,
// or dynamic programming.
bool coursesCompatible(const Course& a, const Course& b) {
    return a.finish <= b.start || b.finish <= a.start;
}

void examineSubsets(const std::vector<Course>& courses,
                    std::size_t index,
                    std::vector<std::size_t>& selected,
                    std::size_t selectedCount,
                    std::size_t& bestCount,
                    long long& bestEnrollments) {
    if (index == courses.size()) {
        long long total = 0;

        for (std::size_t i = 0; i < selectedCount; ++i) {
            for (std::size_t j = i + 1; j < selectedCount; ++j) {
                if (!coursesCompatible(
                        courses[selected[i]],
                        courses[selected[j]])) {
                    return;
                }
            }

            total += courses[selected[i]].enrollments;
        }

        if (selectedCount > bestCount) {
            bestCount = selectedCount;
        }

        if (total > bestEnrollments) {
            bestEnrollments = total;
        }

        return;
    }

    // Exclude this course.
    examineSubsets(
        courses, index + 1, selected, selectedCount,
        bestCount, bestEnrollments
    );

    // Include this course.
    selected[selectedCount] = index;

    examineSubsets(
        courses, index + 1, selected, selectedCount + 1,
        bestCount, bestEnrollments
    );
}

std::size_t optimalCountExhaustive(
    const std::vector<Course>& courses) {
    std::vector<std::size_t> selected(courses.size());

    std::size_t bestCount = 0;
    long long bestEnrollments = 0;

    examineSubsets(
        courses, 0, selected, 0, bestCount, bestEnrollments
    );

    return bestCount;
}

long long optimalEnrollmentsExhaustive(
    const std::vector<Course>& courses) {
    std::vector<std::size_t> selected(courses.size());

    std::size_t bestCount = 0;
    long long bestEnrollments = 0;

    examineSubsets(
        courses, 0, selected, 0, bestCount, bestEnrollments
    );

    return bestEnrollments;
}

void examineRoomPartitions(
    const std::vector<Course>& courses,
    std::size_t index,
    std::vector<std::size_t>& assignment,
    std::size_t roomCount,
    std::size_t& best) {
    if (index == courses.size()) {
        if (roomCount < best) {
            best = roomCount;
        }

        return;
    }

    // Try every existing room, followed by one new room.
    // This avoids repeating partitions that differ only in room names.
    for (std::size_t room = 0; room <= roomCount; ++room) {
        bool valid = true;

        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (assignment[earlier] == room &&
                !coursesCompatible(
                    courses[earlier], courses[index])) {
                valid = false;
                break;
            }
        }

        if (valid) {
            assignment[index] = room;

            std::size_t nextRoomCount = roomCount;

            if (room == roomCount) {
                ++nextRoomCount;
            }

            examineRoomPartitions(
                courses, index + 1, assignment,
                nextRoomCount, best
            );
        }
    }
}

std::size_t minClassroomsExhaustive(
    const std::vector<Course>& courses) {
    std::vector<std::size_t> assignment(courses.size());
    std::size_t best = courses.size();

    examineRoomPartitions(courses, 0, assignment, 0, best);

    return best;
}