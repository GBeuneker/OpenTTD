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
	int GetValueAt(uint16_t index)
	{
		return *m_pointers.at(index);
	}
	int GetSumValue() {
		int sum = 0;
		for (int i = 0; i < m_pointers.size(); ++i)
			sum += *m_pointers.at(i);

		return sum;
	}
	float GetAvgValue() {
		int sum = 0;
		for (int i = 0; i < m_pointers.size(); ++i)
			sum += *m_pointers.at(i);
		return (float)sum / m_pointers.size();
	}
	float GetFilteredAvgValue() {
		int sum = 0;
		int count = 0;
		for (int i = 0; i < m_pointers.size(); ++i)
		{
			int value = *m_pointers.at(i);
			if (value != 0)
			{
				sum += value;
				count++;
			}
		}
		return (float)sum / count;
	}
private:
	std::vector<size_t*> m_pointers;
	char* m_name = "";
};
