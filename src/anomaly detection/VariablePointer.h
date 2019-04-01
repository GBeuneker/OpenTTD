#pragma once

class VariablePointer {
public:
	VariablePointer() {};
	VariablePointer(size_t* pointer, std::string name)
	{
		m_pointers.push_back(pointer);
		m_name = name;
	};
	~VariablePointer() {};
	std::vector<size_t*> GetPointers() { return m_pointers; }
	void AddPointer(size_t* pointer) { m_pointers.push_back(pointer); }
	std::string GetName() { return m_name; }
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
	/// <summary> Gets the average of all the variables with the same id </summary>
	float GetAvgValue() {
		int sum = 0;
		for (int i = 0; i < m_pointers.size(); ++i)
			sum += *m_pointers.at(i);
		return (float)sum / m_pointers.size();
	}
	/// <summary> Gets the average of all the variables which aren't 0 with the same id. </summary>
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
		// Return 0 if all values were 0
		return count == 0 ? 0 : (float)sum / count;
	}
private:
	std::vector<size_t*> m_pointers;
	std::string m_name = "";
};
