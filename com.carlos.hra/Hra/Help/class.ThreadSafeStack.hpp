/** @file class.ThreadSafeStack.hpp
* Trieda ktora sa stara o zasobnik do ktoreho ukladame prijate snimky
* podla : http://stackoverflow.com/questions/790553/thread-safe-c-stack
*/
#pragma once
#include <stack>
#include <boost/thread/lockable_concepts.hpp> 
#include <boost/thread/mutex.hpp>
#include <stdexcept>

template <typename T> class ThreadSafeStack {
public:
	/** 
	* Funkcia ma na vstupe jeden parameter item - to co ideme do zasobnika vlozit
	* @param item - obrazok co vkladame do zasobnika
	* @see DllModulVykreslovania::vykresliObrazokSRozsirenouRealitou(In* in) 
	* @return void
	*/
	void push(const T& item) {
		boost::mutex::scoped_lock lock(m_mutex);
		/*if(size == max_velkost) {
		// Ak velkost je na maxime zastav
		cond.wait( m_mutex );
		}*/
		m_stack.push(item);
	}

	/** 
	* Funkcia nema ziadne parametre - stara sa o vycistenia zasobnika
	* see virtual void frame(FrameData* frame)
	* @return void
	*/
	void clear() {
		boost::mutex::scoped_lock lock(m_mutex);
		while (!m_stack.empty()) {
			T off = m_stack.top();
			m_stack.pop();
			delete off;
		}
		//cond.notify_one();
	}

	/** 
	* Funkcia nema ziadne parametre - stara sa o vyber jedneho objektu zo zasobnika.
	* Pop vola destruktor elementu.
	* @param 
	* @return void
	*/
	/*void pop() {
	boost::mutex::scoped_lock lock(m_mutex);
	m_stack.pop();
	}*/

	/** 
	* Funkcia nema ziadne parametre - vracia vrchol zasobnika
	* Top nevola descruktor elementu.
	* @param 
	* @return T
	*/
	T poll() {
		boost::mutex::scoped_lock lock(m_mutex);
		int size = m_stack.size();
		// Ak velkost klesne na polovicu, oznam to prvemu vlaknu
		/*if(size == max_velkost / 2) {
		cond.notify_one();
		}*/
		// Stack nehadze exception
		if(size == 0) {
			// http://stackoverflow.com/questions/4892108/c-stl-stack-question-why-does-pop-not-throw-an-exception-if-the-stack-is-em
			throw std::out_of_range("Zasobnik prazdny");
		}

		T value;
		value = m_stack.top();
		m_stack.pop();
		return value;
	}

private:
	mutable boost::mutex m_mutex; // mutable
	//boost::condition_variable cond;
	std::stack<T> m_stack;

	// http://stackoverflow.com/questions/6923641/does-boostwait-and-boostcondition-have-to-share-the-same-mutex-object
	//const int m_mutex = 100;
};