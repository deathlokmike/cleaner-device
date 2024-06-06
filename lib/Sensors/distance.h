#pragma once

class Distance
{
private:
    long front = 0;
    long side = 0;

public:
    void setFront(long value);
    void setSide(long value);
    long getFront();
    long getSide();
};