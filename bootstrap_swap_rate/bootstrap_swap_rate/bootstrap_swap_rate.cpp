#include <iostream>
#include <ql/quantlib.hpp>
#include <map>
#pragma warning(disable: 4819)

int main() {
	QuantLib::Date today(5, QuantLib::November, 2019);
	QuantLib::DayCounter day_counter = QuantLib::ActualActual();
	QuantLib::Calendar calendar = QuantLib::TARGET();
	//std::cout << calendar.isBusinessDay(today - 2) << std::endl;

	return 0;
}



//using namespace QuantLib;
//
//int main() {
//
//	try {
//
//		// create common data 
//		Date today(7, Jul, 2017);
//		DayCounter dayCounter = Actual360();
//		Calendar calendar = TARGET();
//		Date settlementDate = calendar.advance(today, Period(2, Days));
//		Natural settlementDays = settlementDate - today;
//		Settings::instance().evaluationDate() = today;
//
//		// create re-linkable handles for discounting and projection curves
//		RelinkableHandle<YieldTermStructure> discountCurve;
//		RelinkableHandle<YieldTermStructure> projectionCurve;
//		// create container for all rate helpers
//		std::vector<boost::shared_ptr<RateHelper>> rateHelpers;
//
//		// create required indices
//		auto eoniaIndex = boost::make_shared<Eonia>();
//		// forward euribor fixings are requested from dual-curve-bootstrapped projection curve
//		auto euriborIndex = boost::make_shared<Euribor6M>(projectionCurve);
//
//
//		// eonia curve
//		// create first cash instrument for eonia curve using deposit rate helper
//		rateHelpers.push_back(boost::make_shared<DepositRateHelper>
//			(Handle<Quote>(boost::make_shared<SimpleQuote>(-0.0036)),
//				Period(1, Days), eoniaIndex->fixingDays(),
//				eoniaIndex->fixingCalendar(), eoniaIndex->businessDayConvention(),
//				eoniaIndex->endOfMonth(), eoniaIndex->dayCounter()));
//
//		// create source data for eonia swaps (period, rate)
//		std::map<Period, Real> eoniaSwapData;
//		eoniaSwapData.insert(std::make_pair(Period(6, Months), -0.00353));
//		eoniaSwapData.insert(std::make_pair(Period(1, Years), -0.00331));
//		eoniaSwapData.insert(std::make_pair(Period(2, Years), -0.00248));
//		eoniaSwapData.insert(std::make_pair(Period(3, Years), -0.00138));
//		eoniaSwapData.insert(std::make_pair(Period(4, Years), -0.0001245));
//		eoniaSwapData.insert(std::make_pair(Period(5, Years), 0.0011945));
//		eoniaSwapData.insert(std::make_pair(Period(7, Years), 0.00387));
//		eoniaSwapData.insert(std::make_pair(Period(10, Years), 0.007634));
//
//		// create other instruments for eonia curve using ois rate helper
//		std::for_each(eoniaSwapData.begin(), eoniaSwapData.end(),
//			[settlementDays, &rateHelpers, &eoniaIndex](std::pair<Period, Real> p) -> void
//			{ rateHelpers.push_back(boost::make_shared<OISRateHelper>(settlementDays,
//				p.first, Handle<Quote>(boost::make_shared<SimpleQuote>(p.second)), eoniaIndex)); });
//
//		// create eonia curve
//		auto eoniaCurve = boost::make_shared<PiecewiseYieldCurve<Discount, LogLinear>>
//			(0, eoniaIndex->fixingCalendar(), rateHelpers, eoniaIndex->dayCounter());
//		eoniaCurve->enableExtrapolation(true);
//		// link discount curve to eonia curve
//		discountCurve.linkTo(eoniaCurve);
//
//		// clear rate helpers container
//		rateHelpers.clear();
//
//
//		// euribor curve
//		// cash part
//		rateHelpers.push_back(boost::make_shared<DepositRateHelper>(Handle<Quote>
//			(boost::make_shared<SimpleQuote>(-0.00273)), Period(6, Months),
//			settlementDays, calendar, euriborIndex->businessDayConvention(),
//			euriborIndex->endOfMonth(), euriborIndex->dayCounter()));
//
//		// fra part
//		rateHelpers.push_back(boost::make_shared<FraRateHelper>(Handle<Quote>
//			(boost::make_shared<SimpleQuote>(-0.00194)), Period(6, Months), euriborIndex));
//
//		// swap part
//		rateHelpers.push_back(boost::make_shared<SwapRateHelper>(Handle<Quote>
//			(boost::make_shared<SimpleQuote>(-0.00119)), Period(2, Years),
//			calendar, Annual, ModifiedFollowing, Actual360(), euriborIndex,
//			// in order to use dual-curve bootstrapping, discount curve handle must
//			// be given as one argument for swap rate helper (along with dummy handle
//			// for quote and dummy zero period for technical reasons)
//			Handle<Quote>(), Period(0, Days), discountCurve));
//
//		rateHelpers.push_back(boost::make_shared<SwapRateHelper>(Handle<Quote>
//			(boost::make_shared<SimpleQuote>(0.00019)), Period(3, Years),
//			calendar, Annual, ModifiedFollowing, Actual360(), euriborIndex,
//			Handle<Quote>(), Period(0, Days), discountCurve));
//
//		rateHelpers.push_back(boost::make_shared<SwapRateHelper>(Handle<Quote>
//			(boost::make_shared<SimpleQuote>(0.00167)), Period(4, Years),
//			calendar, Annual, ModifiedFollowing, Actual360(), euriborIndex,
//			Handle<Quote>(), Period(0, Days), discountCurve));
//
//		rateHelpers.push_back(boost::make_shared<SwapRateHelper>(Handle<Quote>
//			(boost::make_shared<SimpleQuote>(0.00317)), Period(5, Years),
//			calendar, Annual, ModifiedFollowing, Actual360(), euriborIndex,
//			Handle<Quote>(), Period(0, Days), discountCurve));
//
//		rateHelpers.push_back(boost::make_shared<SwapRateHelper>(Handle<Quote>
//			(boost::make_shared<SimpleQuote>(0.00598)), Period(7, Years),
//			calendar, Annual, ModifiedFollowing, Actual360(), euriborIndex,
//			Handle<Quote>(), Period(0, Days), discountCurve));
//
//		rateHelpers.push_back(boost::make_shared<SwapRateHelper>(Handle<Quote>
//			(boost::make_shared<SimpleQuote>(0.00966)), Period(10, Years),
//			calendar, Annual, ModifiedFollowing, Actual360(), euriborIndex,
//			Handle<Quote>(), Period(0, Days), discountCurve));
//
//		// create euribor curve
//		auto euriborCurve = boost::make_shared<PiecewiseYieldCurve<Discount, LogLinear>>
//			(0, euriborIndex->fixingCalendar(), rateHelpers, euriborIndex->dayCounter());
//		euriborCurve->enableExtrapolation();
//		// link projection curve to euribor curve
//		projectionCurve.linkTo(euriborCurve);
//
//
//		// create seasoned vanilla swap
//		Date pastSettlementDate(5, Jun, 2015);
//
//		Schedule fixedSchedule(pastSettlementDate, pastSettlementDate + Period(5, Years),
//			Period(Annual), calendar, Unadjusted, Unadjusted,
//			DateGeneration::Backward, false);
//
//		Schedule floatSchedule(pastSettlementDate, pastSettlementDate + Period(5, Years),
//			Period(Semiannual), calendar, Unadjusted, Unadjusted,
//			DateGeneration::Backward, false);
//
//		VanillaSwap swap(VanillaSwap::Payer, 10000000.0, fixedSchedule, 0.0285,
//			dayCounter, floatSchedule, euriborIndex, 0.0, dayCounter);
//
//		// add required 6M euribor index fixing for floating leg valuation
//		euriborIndex->addFixing(Date(1, Jun, 2017), -0.0025);
//
//		// create pricing engine, request swap pv
//		auto pricer = boost::make_shared<DiscountingSwapEngine>(discountCurve);
//		swap.setPricingEngine(pricer);
//		std::cout << swap.NPV() << std::endl;
//
//	}
//	catch (std::exception& e) {
//		std::cout << e.what() << std::endl;
//	}
//	return 0;
//}