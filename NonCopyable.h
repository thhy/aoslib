#pragma once
class noncopyable
{
public:
	noncopyable();
	virtual ~noncopyable();
private:
	noncopyable(const noncopyable &) = delete;
	noncopyable & operator = (const noncopyable &) = delete;
};

