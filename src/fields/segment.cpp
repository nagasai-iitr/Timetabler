#include "fields/segment.h"

#include <cassert>
#include <string>
#include "global.h"

Segment::Segment(int startSegment, int endSegment) {
    assert((startSegment <= endSegment) && "Start Segment after End Segment!");
    this->startSegment = startSegment;
    this->endSegment = endSegment;
}

bool Segment::operator==(const Segment &other) {
    return ((this->startSegment == other.startSegment) && (this->endSegment && other.endSegment));
}

int Segment::length() {
    return (endSegment - startSegment + 1);
}

bool Segment::isIntersecting(const Segment &other) {
    if(this->startSegment < other.startSegment) {
        return !(this->endSegment < other.startSegment);    
    }
    else if(this->startSegment > other.startSegment) {
        return !(this->startSegment > other.endSegment);
    }
    return true;
}

FieldType Segment::getType() {
    return FieldType::segment;
}

std::string Segment::toString() {
    return std::to_string(startSegment) + std::to_string(endSegment);
}

std::string Segment::getTypeName() {
    return "Segment";
}