/********************************************************************
 * $Id: StRichAnalogToDigitalConverter.h,v 1.4 2000/03/12 23:56:33 lasiuk Exp $
 *
 * Description:
 *   StRichAnalogToDigitalConverter is the function object containing
 *   the algorithm that transforms the simulated analog signal 
 *   to a digital output. 
 *    
 *   StRichAnalogToDigitalConverter is used like a normal function,
 *   i.e. StRichAnalogToDigitalConverter(double);  
 * 
 *   StRichAnalogToDigitalConverter takes an analog signal
 *   and converts it to an ADC count, depending on the
 *   given adc factor, pedestal and adc range.
 *
 ********************************************************************
 * $Log: StRichAnalogToDigitalConverter.h,v $
 * Revision 1.4  2000/03/12 23:56:33  lasiuk
 * new coordinate system
 * exchange MyRound with inline templated funtion
 *
 * Revision 1.3  2000/02/08 16:22:20  lasiuk
 * use dbs
 * systemOfUnits now used
 *
 * Revision 1.2  2000/01/25 22:02:19  lasiuk
 * Second Revision
 *
 * Revision 1.1  2000/01/18 21:32:00  lasiuk
 * Initial Revision
 *
 *   revision history:
 *     - 7/22/1999 created the class, Alexandre Nevski.
 *     - 8/24/1999 initial implementation
 *     - 8/31/1999 final ADC counts implementation
 *
 ********************************************************************/
#ifndef ST_RICH_ANALOG_TO_DIGITAL_CONVERTER_H
#define ST_RICH_ANALOG_TO_DIGITAL_CONVERTER_H

#include <functional>
#ifndef ST_NO_NAMESPACES
using std::unary_function;
#endif

#ifndef ST_NO_NAMESPACES
//namespace StRichRawData {
#endif
#include "StRichRrsMacros.h"
class StRichPhysicsDb;

class StRichAnalogToDigitalConverter : public unary_function<double,double> {
public:
    StRichAnalogToDigitalConverter();
    ~StRichAnalogToDigitalConverter();
    
    //SRichAnalogToDigitalConverter(const StRichAnalogToDigitalConverter&);
    //operator=RichAnalogToDigitalConverter(StRichAnalogToDigitalConverter&);

    int operator()(double) const;

    void setAddPedestal(int);
    
private:
    StRichPhysicsDb*  mPhysicsDb;
    int               mMaxADC;
    int               mPedestal;
    double            mAdcConversion;
    double            mElectronCharge;
    int               mAddPedestal;
};
inline
void StRichAnalogToDigitalConverter::setAddPedestal(int v) {mAddPedestal = v;}

#ifndef ST_NO_NAMESPACES
//} 
#endif

#endif
