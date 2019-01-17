#pragma once

class VariablePointer {
public:
	VariablePointer() {};
	VariablePointer(size_t* pointer, char* name)
	{
		m_pointer = pointer;
		m_name = name;
	};
	~VariablePointer() {};
	size_t* GetPointer() { return m_pointer; }
	char* GetName() { return m_name; }
	int GetValue() { return *m_pointer; }
private:
	size_t* m_pointer;
	char* m_name;
};
