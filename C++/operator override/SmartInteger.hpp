#include <iostream>

class SmartInteger {
	private:
		int num;
	public:
		SmartInteger();
		SmartInteger(int i);
		const int getValue() const;
		//overloading
		friend std::ostream& operator<<(std::ostream& os, const SmartInteger& i);
		const bool operator<(const SmartInteger& i) const;
		const bool operator>(const SmartInteger& i) const;
		const bool operator<=(const SmartInteger& i) const;
		const bool operator>=(const SmartInteger& i) const;
		const bool operator==(const SmartInteger& i) const;
		const SmartInteger operator+(const SmartInteger& i) const;
		const SmartInteger operator-(const SmartInteger& i) const;
		const SmartInteger operator*(const SmartInteger& i) const;
		SmartInteger operator+=(const SmartInteger i);
		SmartInteger operator-=(const SmartInteger& i);
		SmartInteger operator*=(const SmartInteger& i);
		SmartInteger operator++();
		SmartInteger operator--();
};
