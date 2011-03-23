
#ifndef KIWI_CORE_STATICNODE_HPP
#define KIWI_CORE_STATICNODE_HPP

#include <vector>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/mpl/int.hpp>

#include "kiwi/core/StaticReaderPort.hpp"
#include "kiwi/core/StaticWriterPort.hpp"
#include "kiwi/core/StaticDataPort.hpp"
#include "kiwi/core/Node.hpp"

#define ReaderListMacro boost::fusion::vector
#define WriterListMacro boost::fusion::vector
#define DataListMacro boost::fusion::vector

namespace kiwi{
namespace core{


template<typename TLayout> class StaticNode;

template<typename NodeType> struct PortNodeSetter{
  PortNodeSetter(NodeType* node) : _node(node){}
  template<typename T> void operator()(const T& port) const {
    const_cast<T&>(port).setNode(_node); }
  NodeType* _node;
};





// ------------------------------------------------------------- Port Wrappers -
// -----------------------------------------------------------------------------
template<int i, class LayoutType> struct _ReaderWrapperLoop{
  static void exec(LayoutType& layout){
    Debug::print() << (int) i-1;
    _ReaderWrapperLoop<i-1,LayoutType>::exec(layout);
    layout._dynReaderPorts.push_back(&boost::fusion::at_c<i-1>(layout._readerPorts));
  }
};
template<class LayoutType> struct _ReaderWrapperLoop<0,LayoutType>{
  static void exec(LayoutType& layout){ }
};
template<int i, class LayoutType> struct _WriterWrapperLoop{
  static void exec(LayoutType& layout){
    _WriterWrapperLoop<i-1,LayoutType>::exec(layout);
    layout._dynWriterPorts.push_back(&boost::fusion::at_c<i-1>(layout._writerPorts));
  }
};
template<class LayoutType> struct _WriterWrapperLoop<0,LayoutType>{
  static void exec(LayoutType& layout){ }
};
template<int i, class LayoutType> struct _DataWrapperLoop{
  static void exec(LayoutType& layout){
    _DataWrapperLoop<i-1,LayoutType>::exec(layout);
    layout._dynDataPorts.push_back(&boost::fusion::at_c<i-1>(layout._dataPorts));
  }
};
template<class LayoutType> struct _DataWrapperLoop<0,LayoutType>{
  static void exec(LayoutType& layout){ }
};
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


template<typename TReaderList, typename TWriterList, typename TDataList>
struct StaticNodeLayout{
  // typedefs
  typedef TReaderList ReaderList;
  typedef TWriterList WriterList;
  typedef TDataList DataList;
  typedef StaticNodeLayout<ReaderList,WriterList,DataList> Self;
  // number of ports
  static const portIndex_t NbReaderPorts
    = boost::mpl::size<ReaderList>::value;
  static const portIndex_t NbWriterPorts
    = boost::mpl::size<WriterList>::value;
  static const portIndex_t NbDataPorts
    = boost::mpl::size<DataList>::value;

  /**
   * @brief Constructor.
   */ 
  StaticNodeLayout(kiwi::core::StaticNode<Self>* node){
    ScopedBlockMacro("StaticNodeLayout::constructor")
    // set each port's associated Node to node  
    boost::fusion::for_each(
      boost::fusion::as_vector(_readerPorts), PortNodeSetter<Node>(node) );
    boost::fusion::for_each(
      boost::fusion::as_vector(_writerPorts), PortNodeSetter<Node>(node) );
    boost::fusion::for_each(
      boost::fusion::as_vector(_dataPorts)  , PortNodeSetter<Node>(node) );
    // then fill the vectors containing higher level access to ports
    _ReaderWrapperLoop<NbReaderPorts,Self>::exec(*this);
    _WriterWrapperLoop<NbWriterPorts,Self>::exec(*this);
    _DataWrapperLoop<NbDataPorts,Self>::exec(*this);
  }

  ReaderList _readerPorts;
  WriterList _writerPorts;
  DataList   _dataPorts;

  std::vector<ReaderPort*> _dynReaderPorts;
  std::vector<WriterPort*> _dynWriterPorts;
  std::vector<DataPort*> _dynDataPorts;

};


template<typename TLayout>
class StaticNode : public kiwi::core::Node
{
public:
  typedef TLayout Layout;

  StaticNode() : _layout(this){}
  static const portIndex_t NbReaderPorts
    = boost::mpl::size<typename Layout::ReaderList>::value;
  static const portIndex_t NbWriterPorts
    = boost::mpl::size<typename Layout::WriterList>::value;
  static const portIndex_t NbDataPorts
    = boost::mpl::size<typename Layout::DataList>::value;

  // to get the low level port types
  template<int i> struct staticReaderPortInfo{
    typedef typename boost::mpl::at<typename Layout::ReaderList,boost::mpl::int_<i> >::type type;
  };
  template<int i> struct staticWriterPortInfo{
    typedef typename boost::mpl::at<typename Layout::WriterList,boost::mpl::int_<i> >::type type;
  };
  template<int i> struct staticDataPortInfo{
    typedef typename boost::mpl::at<typename Layout::DataList,boost::mpl::int_<i> >::type type;
  };

  // to get the low level port instances
  template<int i> typename staticReaderPortInfo<i>::type& staticReaderPort() {
    return boost::fusion::at_c<i>(_layout._readerPorts);
  }
  template<int i> typename staticWriterPortInfo<i>::type& staticWriterPort() {
    return boost::fusion::at_c<i>(_layout._writerPorts);
  }
  template<int i> typename staticDataPortInfo<i>::type& staticDataPort() {
    return boost::fusion::at_c<i>(_layout._dataPorts);
  }

  // to get the high level port instances
  ReaderPort& readerPort(portIndex_t i = 0) const {
    return *_layout._dynReaderPorts[i];
  }
  WriterPort& writerPort(portIndex_t i = 0) const {
    return *_layout._dynWriterPorts[i];
  }
  DataPort& dataPort(portIndex_t i = 0) const {
    return *_layout._dynDataPorts[i];
  }
  
  kiwi::portIndex_t nbReaderPorts() const{
    return NbReaderPorts;
  }

  kiwi::portIndex_t nbWriterPorts() const{
    return NbWriterPorts;
  }

  kiwi::portIndex_t nbDataPorts() const{
    return NbDataPorts;
  }


protected:
  Layout _layout;

};





}//namespace
}//namespace


#endif

