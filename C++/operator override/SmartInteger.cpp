#include <iostream>
#include <exception>
#include "SmartInteger.hpp"
#include <climits>

SmartInteger::SmartInteger() {
	num = 0;
}

SmartInteger::SmartInteger(int i) {
	num = i;
}

const int SmartInteger::getValue() const{
	return this->num;
}

std::ostream& operator<<(std::ostream& os, const SmartInteger& i) {
	os << i.num;
	return os;
}

const bool SmartInteger::operator<(const SmartInteger& i) const{
	return this->getValue() < i.getValue();
}

const bool SmartInteger::operator>(const SmartInteger& i) const{
	return this->getValue() > i.getValue();
}

const bool SmartInteger::operator<=(const SmartInteger& i) const{
	return this->getValue() <= i.getValue();
}

const bool SmartInteger::operator>=(const SmartInteger& i) const{
	return this->getValue() >= i.getValue();
}

const bool SmartInteger::operator==(const SmartInteger& i) const{
	return this->getValue() == i.getValue();
}

const SmartInteger SmartInteger::operator+(const SmartInteger& i) const {
	if(this->getValue() > 0 && i.getValue() > INT_MAX - this->getValue()) {
		throw std::overflow_error("error");
	}
	if(this->getValue() < 0 && i.getValue() < INT_MIN - this->getValue()) {
		throw std::underflow_error("error");
	SmartInteger res;
	res.num = this->getValue() + i.getValue();
	return res;
	}
}

const SmartInteger SmartInteger::operator-(const SmartInteger& i) const{
	if(this->getValue() < 0 && i.getValue() > INT_MAX + this->getValue()) {
		throw std::overflow_error("error");
	}
	if(this->getValue() > 0 && i.getValue() < INT_MIN + this->getValue()) {
		throw std::underflow_error("error");
	}
	SmartInteger res;
	res.num = this->getValue() + i.getValue();
	return res;
}

const SmartInteger SmartInteger::operator*(const SmartInteger& i) const {
	if (this->getValue() > 0 && i.getValue() > 0) {
		if (this->getValue() > INT_MAX/i.getValue()) {
			throw std::overflow_error("error");
		}
	}
	if (this->getValue() > 0 && i.getValue() < 0) {
		if (this->getValue() > INT_MIN/i.getValue()) {
			throw std::underflow_error("error");
		}
	}
	if(this->getValue() < 0 && i.getValue() < 0) {
		if (this->getValue() < INT_MAX/i.getValue()) {
			throw std::overflow_error("error");
		}
	}
	if(this->getValue() < 0 && i.getValue() > 0) {
		if (this->getValue() < INT_MIN/i.getValue()) {
			throw std::underflow_error("error");
		}
	}
	if (this->getValue() == -1 && i.getValue() == INT_MIN) {
		throw std::overflow_error("error");
	}
	if (this->getValue() == INT_MIN && i.getValue() == -1) {
		throw std::overflow_error("error");
	}
	SmartInteger res;
	res.num = num*i.num;
	return res;
}

SmartInteger SmartInteger::operator+=(const SmartInteger i){
	*this = *this + i;
	return *this;
}

SmartInteger SmartInteger::operator-=(const SmartInteger& i){
	*this = *this - i;
	return *this;
}

SmartInteger SmartInteger::operator*=(const SmartInteger& i){
	*this = *this * i;
	return *this;
}

SmartInteger SmartInteger::operator++(){
	*this = *this + 1;
	return *this;
}

SmartInteger SmartInteger::operator--(){
	*this = *this - 1;
	return *this;
}
