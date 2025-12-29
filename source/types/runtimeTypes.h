/**
 * @file runtimeTypes.h
 * @brief Type definitions for runtime-related operations.
 *
 * This header provides type aliases used by the RuntimeService
 * and other components that work with runtime execution.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_TYPES_RUNTIMETYPES_H
#define NSBACI_TYPES_RUNTIMETYPES_H

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

using ThreadID = unsigned long long int;

using Priority = unsigned long int;

enum class ThreadState { Waiting, Ready, Running, Blocked, IO };

/**
 * @struct Address
 * @brief Represents a memory address in the runtime.
 * It is a wrapper for a raw type, so it defines operators to work directly with
 * the wrapper instead of the member
 */
struct Address {
  unsigned long long int value;

  // Default constructor
  Address() : value(0) {}

  // Value constructor
  explicit Address(unsigned long long int val) : value(val) {}

  // Comparison operators
  bool operator==(const Address& other) const { return value == other.value; }
  bool operator!=(const Address& other) const { return value != other.value; }
  bool operator<(const Address& other) const { return value < other.value; }
  bool operator>(const Address& other) const { return value > other.value; }
  bool operator<=(const Address& other) const { return value <= other.value; }
  bool operator>=(const Address& other) const { return value >= other.value; }

  // Arithmetic operators with Address
  Address operator+(const Address& other) const {
    return Address(value + other.value);
  }
  Address operator-(const Address& other) const {
    return Address(value - other.value);
  }

  // Arithmetic operators with offset
  Address operator+(unsigned long long int offset) const {
    return Address(value + offset);
  }
  Address operator-(unsigned long long int offset) const {
    return Address(value - offset);
  }

  // Compound assignment operators
  Address& operator+=(const Address& other) {
    value += other.value;
    return *this;
  }
  Address& operator-=(const Address& other) {
    value -= other.value;
    return *this;
  }
  Address& operator+=(unsigned long long int offset) {
    value += offset;
    return *this;
  }
  Address& operator-=(unsigned long long int offset) {
    value -= offset;
    return *this;
  }

  // Increment/decrement operators
  Address& operator++() {  // Pre-increment
    ++value;
    return *this;
  }
  Address operator++(int) {  // Post-increment
    Address temp(*this);
    ++value;
    return temp;
  }
  Address& operator--() {  // Pre-decrement
    --value;
    return *this;
  }
  Address operator--(int) {  // Post-decrement
    Address temp(*this);
    --value;
    return temp;
  }

  // Conversion operator (explicit to avoid implicit conversions)
  explicit operator unsigned long long int() const { return value; }
};

}  // namespace nsbaci::types

#endif  // NSBACI_TYPES_RUNTIMETYPES_H
