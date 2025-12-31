#include <iostream>
#include <array>
#include <cstddef>
#include <cassert>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cstring>
#include <cstdint>

const std::uint8_t fieldSize = 16;
const std::array<std::uint8_t, fieldSize> finishField = {
    1, 2, 3, 4,
    5, 6, 7, 8,
    9, 10, 11, 12,
    13, 14, 15, 0
};

enum class Direction {
    Start,
    Left,
    Right,
    Down,
    Up
};

Direction inverse(Direction d) {
    switch(d) {
        case Direction::Left: 
            return Direction::Right;
        case Direction::Right:
            return Direction::Left;
        case Direction::Up:
            return Direction::Down;
        case Direction::Down: 
            return Direction::Up;
        default: 
            return Direction::Start;
    }
}

class GameState {
private:
    std::array<std::uint8_t, fieldSize> _field;
    std::int8_t _emptyPos;
    std::size_t _dimension;

    std::size_t getInversionCount() const {
        std::size_t inversionCounter = 0;
        for (auto i = 0; i < fieldSize - 1; i++) {
            for (auto j = i + 1; j < fieldSize; j++) {
                inversionCounter += static_cast<std::size_t>(
                    _field[i] > _field[j] && _field[i] != 0 && _field[j] != 0
                );
            }
        }
        return inversionCounter;
    }

    bool canMoveByOffset(std::int8_t offset) const {
        std::int8_t newPos = _emptyPos + offset;
        if (newPos < 0 || newPos >= static_cast<std::int8_t>(fieldSize)) {
            return false;
        }

        std::int8_t oldRow = _emptyPos / _dimension;
        std::int8_t newRow = newPos / _dimension;

        if (std::abs(newRow - oldRow) > 1) {
            return false;
        }

        return true;
    }

    GameState moveByOffset(std::int8_t offset) const {
        assert(canMoveByOffset(offset));

        GameState newState(*this);
        std::swap(
            newState._field[_emptyPos],
            newState._field[_emptyPos + offset]
        );
        newState._emptyPos += offset;

        return newState;
    }

public:
    explicit GameState(
        const std::array<std::uint8_t, fieldSize> &field
    ) : _field(field) {
        _emptyPos = -1;
        for (int i = 0; i < fieldSize; i++) {
            if (field[i] == 0) {
                _emptyPos = i;
            }
        }

        assert(_emptyPos != -1);

        _dimension = static_cast<std::size_t>(
            std::sqrt(fieldSize)
        );
    }

    GameState() = default;

    bool operator==(const GameState &other) const {
        return _field == other._field;
    }

    const std::array<std::uint8_t, fieldSize> &field() const {
        return _field;
    }

    bool isComplete() const {
        return _field == finishField;
    }

    bool isSolvable() const {
        if (fieldSize % 2 == 1) {
            return (getInversionCount() % 2 == 0);
        }
        
        std::size_t a = getInversionCount();
        std::size_t b = (_emptyPos / _dimension) + 1;

        return (a + b) % 2 == 0;
    }

    bool canMove(Direction d) const {
        switch (d) {
        case Direction::Left:
            return canMoveByOffset(+1);
        case Direction::Right:
            return canMoveByOffset(-1);
        case Direction::Up:
            return canMoveByOffset(+_dimension);
        case Direction::Down:
            return canMoveByOffset(-_dimension);
        default:
            return false;
        }
    }

    GameState move(Direction d) const {
        switch (d) {
        case Direction::Left:
            return moveByOffset(+1);
        case Direction::Right:
            return moveByOffset(-1);
        case Direction::Up:
            return moveByOffset(+_dimension);
        case Direction::Down:
            return moveByOffset(-_dimension);
        default:
            return *this;
        }
    }

    std::size_t manhattanDistance() const {
        std::size_t dist = 0;
        for (std::size_t i = 0; i < fieldSize; i++) {
            std::uint8_t value = _field[i];
            if (value == 0) continue;

            std::size_t targetRow = (value - 1) / _dimension;
            std::size_t targetCol = (value - 1) % _dimension;
            std::size_t currentRow = i / _dimension;
            std::size_t currentCol = i % _dimension;

            dist += std::abs(static_cast<int>(currentRow) - static_cast<int>(targetRow));
            dist += std::abs(static_cast<int>(currentCol) - static_cast<int>(targetCol));
        }
        return dist;
    }
};

class GameStateHash {
public:
    std::size_t operator()(const GameState &state) const {
        const auto &field = state.field();
        std::size_t hash = 0;
        for (auto x : field) {
            hash = hash * 31 + x;
        }
        return hash;
    }
};

struct Node {
public:
    GameState state;
    std::size_t g;
    std::size_t f;
public:
    Node(
        const GameState &s,
        std::size_t g_val, 
        std::size_t f_val
    ) : state(s), g(g_val), f(f_val) {}

    bool operator > (const Node &other) const {
        if (f != other.f) {
            return f > other.f;
        }
        return g > other.g;
    }
};

std::pair<
    std::size_t,
    std::vector<Direction>
>
getSolution(const std::array<std::uint8_t, fieldSize> &field) {
    std::vector<Direction> stepsPath;

    GameState startState(field);
    if (startState.isSolvable() == false) {
        return {0, stepsPath};
    }

    std::unordered_map<GameState, std::pair<GameState, Direction>, GameStateHash> cameFrom;
    std::unordered_map<GameState, std::size_t, GameStateHash> gScore;

    auto cmp = [](const Node &a, const Node &b) { return a > b; };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> openSet(cmp);

    gScore[startState] = 0;
    openSet.push(Node(startState, 0, startState.manhattanDistance()));

    const Direction allDirections[] = {
        Direction::Left,
        Direction::Right,
        Direction::Up,
        Direction::Down
    };

    while (openSet.empty() == false) {
        Node current = openSet.top();
        openSet.pop();

        if (current.state.isComplete()) {
            GameState state = current.state;
            while (cameFrom.find(state) != cameFrom.end()) {
                auto &[parent, direction] = cameFrom[state];
                stepsPath.push_back(direction);
                state = parent;
            }
            std::reverse(stepsPath.begin(), stepsPath.end());
            return {stepsPath.size(), stepsPath};
        }

        if (current.g > gScore[current.state]) {
            continue;
        }

        for (auto d : allDirections) {
            if (current.state.canMove(d) == false) {
                continue;
            }

            GameState neighbor = current.state.move(d);
            std::size_t tentative_g = current.g + 1;

            auto it = gScore.find(neighbor);
            if (it == gScore.end() || tentative_g < it->second) {
                cameFrom[neighbor] = {current.state, d};
                gScore[neighbor] = tentative_g;
                std::size_t f = tentative_g + neighbor.manhattanDistance();
                openSet.push(Node(neighbor, tentative_g, f));
            }
        }
    }

    return {0, stepsPath};
}

std::string directionsToString(
    const std::vector<Direction> &path
) {
    std::string result;
    result.resize(path.size());

    for (std::size_t i = 0; i < path.size(); ++i) {
        switch (path[i]) {
            case Direction::Up:    result[i] = 'U'; break;
            case Direction::Down:  result[i] = 'D'; break;
            case Direction::Left:  result[i] = 'L'; break;
            case Direction::Right: result[i] = 'R'; break;
        }
    }

    return result;
}

int main() {
    std::array<std::uint8_t, fieldSize> gameField;
    for (int i = 0; i < fieldSize; ++i) {
        int x;
        std::cin >> x;
        gameField[i] = static_cast<std::uint8_t>(x);
    }

    auto [length, moves] = getSolution(gameField);
    if (length == 0) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << length << std::endl;
        std::cout << directionsToString(moves) << std::endl;
    }

    return 0;
}