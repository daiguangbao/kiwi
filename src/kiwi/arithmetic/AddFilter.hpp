#ifndef KIWI_ARITHMETIC_ADDFILTER_HPP
#define KIWI_ARITHMETIC_ADDFILTER_HPP

#include "kiwi/core/Commons.hpp"
#include "kiwi/generic/NumberContainer.hpp"
#include "kiwi/utils/types.hpp"

namespace kiwi{
namespace arithmetic{

#define KIWI_ADDFILTER_TYPE kiwi::float_t

class AddFilter : public kiwi::core::Filter
{
public:
	AddFilter(){
		_result = new kiwi::generic::NumberContainer<KIWI_ADDFILTER_TYPE>();
		addReaderInputPort(); // A
		addReaderInputPort(); // B
		addReaderOutputPort( _result );
	}

	void process(){
		ScopedBlockMacro( __scrop,"AddFilter::process()" );
		if( _result ){
			Debug::print() << "get data A\n";
			typedef kiwi::generic::NumberContainerInterface<KIWI_ADDFILTER_TYPE> NumberContainer_T;
			
			NumberContainer_T* ca = readerInputPort(0).connectedOutput()->getContainer<NumberContainer_T>();

			Debug::print() << "get data B\n";
			NumberContainer_T* cb = readerInputPort(1).connectedOutput()->getContainer<NumberContainer_T>();

			
			Debug::print() << "get value A\n";
			int A = 0;
			if(ca) A = ca->getValue();
			Debug::print() << "get value B\n";
			int B = 0;
			if(cb) B = cb->getValue();

			_result->setValue( A + B );

		}
	}

	template<typename TypeR>
	void processTemplate()
	{

	}

	kiwi::Tags readerInputTags(kiwi::portIndex_t index) const{
		return kiwi::Tags("#number#any");// TODO
	}

	void layoutChanged(){
		if( readerInputPort(0).isConnected()
		&& readerInputPort(1).isConnected() ){
			if( _result == 0 ){
				_result = new kiwi::generic::NumberContainer<KIWI_ADDFILTER_TYPE>( 0 );
				setPortContainer( readerOutputPort(0), _result );
				setPortEnabled( readerOutputPort(0), false );
			}
		}else{
			if( _result != 0){
				delete _result;
				_result = 0;
				setPortEnabled( readerOutputPort(0), false );
			}
		}
	}


protected:
	kiwi::generic::NumberContainerInterface<KIWI_ADDFILTER_TYPE>* _result;
	template<typename T1,typename T2> void process2();
};


}//namespace
}//namespace


#endif