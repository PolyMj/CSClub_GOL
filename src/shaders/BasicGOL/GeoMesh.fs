#version 430 core
// 410 mac

layout(location=0) out uint lifeBoolOut;

void main() {
    // Set all overlapping pixels to alive
    lifeBoolOut = uint(1);
}
