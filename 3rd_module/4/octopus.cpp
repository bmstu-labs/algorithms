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

class GameState {
private:
    std::array<std::uint8_t, fieldSize> _field;
    std::int8_t _emptyPos;
    std::size_t _rowCount;
private:
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
        std::int8_t newPos =  _emptyPos + offset;
        if (newPos < 0 || newPos >= static_cast<std::int8_t>(fieldSize)) {
            return false;
        }

        std::int8_t oldRow = _emptyPos / _rowCount;
        std::int8_t newRow = newPos / _rowCount;

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

        return std::move(newState);
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

        _rowCount = static_cast<std::size_t>(
            std::sqrt(fieldSize)
        );
    }
public:
    bool operator == (const GameState &other) const {
        return _field == other._field;
    }

public:
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
        std::size_t b = (_emptyPos / _rowCount) + 1;

        return (a + b) % 2 == 0;
    }

    bool canMove(Direction d) {
        switch (d) {
        case Direction::Left:
            return canMoveByOffset(+1);
        case Direction::Right:
            return canMoveByOffset(-1);
        case Direction::Up:
            return canMoveByOffset(+_rowCount);
        case Direction::Down:
            return canMoveByOffset(-_rowCount);
        }

        return false;
    }

    GameState move(Direction d) {
        switch (d) {
        case Direction::Left:
            return moveByOffset(+1);
        case Direction::Right:
            return moveByOffset(-1);
        case Direction::Up:
            return moveByOffset(+_rowCount);
        case Direction::Down:
            return moveByOffset(-_rowCount);
        }
    }
};

class GameStateHash {
public:
    std::size_t operator () (const GameState &state) const {
        std::size_t hash = 0;
        std::memcpy(&hash, state.field().data(), sizeof(hash));
        return hash;
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

    std::unordered_map<GameState, Direction, GameStateHash> visited;
    visited[startState] = Direction::Start;

    std::queue<GameState> q;
    q.push(startState);

    static constexpr Direction allDirections[] = {
        Direction::Left,
        Direction::Right,
        Direction::Up,
        Direction::Down
    };

    while (1) {
        GameState state = q.front();
        q.pop();

        if (state.isComplete()) {
            break;
        }

        for (auto d : allDirections) {
            if (state.canMove(d) == false) {
                continue;
            }

            GameState newState = state.move(d);
            if (visited.contains(newState) == false) {
                visited[newState] = d;
                q.push(newState);
            }
        }
    }

    GameState state(finishField);
    while (visited[state] != Direction::Start) {
        Direction d = visited[state];

        switch (d) {
            case Direction::Left:
                stepsPath.push_back(Direction::Right);
                state = state.move(Direction::Right);
                break;

            case Direction::Right:
                stepsPath.push_back(Direction::Left);
                state = state.move(Direction::Left);
                break;

            case Direction::Up:
                stepsPath.push_back(Direction::Down);
                state = state.move(Direction::Down);
                break;

            case Direction::Down:
                stepsPath.push_back(Direction::Up);
                state = state.move(Direction::Up);
                break;
        }
    }

    std::reverse(stepsPath.begin(), stepsPath.end());
    return {stepsPath.size(), stepsPath};
}

std::string directionsToString(
    const std::vector<Direction> &path
) {
    std::string result;
    result.resize(path.size());

    for (std::size_t i = 0; i < path.size(); ++i) {
        switch (path[i]) {
            case Direction::Up:
                result[i] = 'D';
                break;
            case Direction::Down:
                result[i] = 'U';
                break;
            case Direction::Left:
                result[i] = 'R';
                break;
            case Direction::Right:
                result[i] = 'L';
                break;
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
    }
    else {
        std::cout << length << std::endl;
        std::cout << directionsToString(moves) << std::endl;
    }

    return 0;
}