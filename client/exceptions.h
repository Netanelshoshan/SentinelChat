#ifndef EXCEPTIONS_HEADER
#define EXCEPTIONS_HEADER

#include <exception>

class UserNotInList : public std::exception {
public:
    const char *what() const throw() {
        return "User not in users list";
    }
};

class KeyNotAvailable : public std::exception {
public:
    [[nodiscard]]const char *what() const noexcept override {
        return "Key is not available for this user.";
    }
};

class UserNameNotFound : public std::exception {
public:
    const char *what() const throw() {
        return "User name not found.";
    }
};

class UserIdNotFound : public std::exception {
public:
    const char *what() const noexcept override {
        return "User id not found.";
    }
};


class FileNotFound : public std::exception {
public:
    const char *what() const noexcept override {
        return "me.info file not found.";
    }
};

class PubKeyNotFound : public std::exception {
public:
    [[nodiscard]] const char *what() const noexcept override {
        return "Public Key not found.";
    }
};

class SymKeyNotFound : public std::exception {
public:
    [[nodiscard]]const char *what() const noexcept override {
        return "Symmetric Key not found.";
    }
};
class IncompatibleServerVersion : public std::exception {
public:
    [[nodiscard]]const char *what() const noexcept override {
        return "Incompatible server version.";
    }
};
class UserNotRegistered : public std::exception {
public:
    [[nodiscard]]const char *what() const noexcept override {
        return "You're not registered. Please register first.";
    }
};

#endif