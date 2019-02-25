#pragma once

class VariablePointer {
public:
	VariablePointer() {};
	VariablePointer(size_t* pointer, char* name)
	{
		m_pointers.push_back(pointer);
		m_name = name;
	};
	~VariablePointer() {};
	std::vector<size_t*> GetPointers() { return m_pointers; }
	void AddPointer(size_t* pointer) { m_pointers.push_back(pointer); }
	char* GetName() { return m_name; }
	int GetValue() {
		int sum = 0;
		for (int i = 0; i < m_pointers.size(); ++i)
			sum += *m_pointers.at(i);

		return sum;
	}
	int GetAvgValue() {
		int sum = 0;
		for (int i = 0; i < m_pointers.size(); ++i)
			sum += *m_pointers.at(i);
		return sum / m_pointers.size();
	}
private:
	std::vector<size_t*> m_pointers;
	char* m_name = "";
};
