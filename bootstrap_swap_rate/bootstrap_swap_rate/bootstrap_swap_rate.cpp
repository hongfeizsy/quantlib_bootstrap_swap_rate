#include <iostream>
#include <ql/quantlib.hpp>
#include <boost/timer.hpp>
#pragma warning(disable: 4819)

int main() {
	boost::timer timer;
	QuantLib::Calendar calendar = QuantLib::Germany(QuantLib::Germany::FrankfurtStockExchange);
	QuantLib::Date todayDate = QuantLib::Date(11, QuantLib::July, 2009);
	std::cout << todayDate.weekday() << std::endl;;
	QuantLib::Integer fixingDays = 2;
	QuantLib::Date settlementDate = calendar.advance(todayDate, fixingDays, QuantLib::Days);   // QuantLib::Calendar::advance can produce the working day.
	QuantLib::Settings::instance().evaluationDate() = todayDate;

	std::vector<std::pair<QuantLib::Period, QuantLib::Rate>> depositQuotes = {
		std::make_pair(QuantLib::Period(6, QuantLib::Months), 0.012680)
	};

	std::vector<std::pair<std::pair<QuantLib::Period, QuantLib::Period>, QuantLib::Rate>> fraQuotes = {
		std::make_pair(std::make_pair(QuantLib::Period(1, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.011950),
		std::make_pair(std::make_pair(QuantLib::Period(2, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.011880),
		std::make_pair(std::make_pair(QuantLib::Period(3, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.011960),
		std::make_pair(std::make_pair(QuantLib::Period(4, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.012320),
		std::make_pair(std::make_pair(QuantLib::Period(5, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.012445),
		std::make_pair(std::make_pair(QuantLib::Period(6, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.012695),
		std::make_pair(std::make_pair(QuantLib::Period(7, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.013450),
		std::make_pair(std::make_pair(QuantLib::Period(8, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.013970),
		std::make_pair(std::make_pair(QuantLib::Period(9, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.014690),
		std::make_pair(std::make_pair(QuantLib::Period(10, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.015510),
		std::make_pair(std::make_pair(QuantLib::Period(11, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.016260),
		std::make_pair(std::make_pair(QuantLib::Period(12, QuantLib::Months), QuantLib::Period(6, QuantLib::Months)), 0.017445)
	};

	std::vector<std::pair<QuantLib::Period, QuantLib::Rate>> swapQuotes = {
		std::make_pair(QuantLib::Period(2, QuantLib::Years), 0.016665),
		std::make_pair(QuantLib::Period(3, QuantLib::Years), 0.021310),
		std::make_pair(QuantLib::Period(4, QuantLib::Years), 0.024900),
		std::make_pair(QuantLib::Period(5, QuantLib::Years), 0.027710),
		std::make_pair(QuantLib::Period(6, QuantLib::Years), 0.030030),
		std::make_pair(QuantLib::Period(7, QuantLib::Years), 0.031870),
		std::make_pair(QuantLib::Period(8, QuantLib::Years), 0.033340),
		std::make_pair(QuantLib::Period(9, QuantLib::Years), 0.034565),
		std::make_pair(QuantLib::Period(10, QuantLib::Years), 0.035640),
		std::make_pair(QuantLib::Period(11, QuantLib::Years), 0.036630),
		std::make_pair(QuantLib::Period(12, QuantLib::Years), 0.037525),
		std::make_pair(QuantLib::Period(15, QuantLib::Years), 0.039580),
		std::make_pair(QuantLib::Period(20, QuantLib::Years), 0.040980),
		std::make_pair(QuantLib::Period(25, QuantLib::Years), 0.040790),
		std::make_pair(QuantLib::Period(30, QuantLib::Years), 0.040220)
	};

	QuantLib::BusinessDayConvention businessDayConvention = QuantLib::ModifiedFollowing;
	QuantLib::DayCounter depoDayCounter = QuantLib::Actual360();
	std::vector<boost::shared_ptr<QuantLib::RateHelper>> depoFraSwapInstruments;
	for (size_t i = 0; i < depositQuotes.size(); i++) {
		depoFraSwapInstruments.push_back(
			boost::make_shared<QuantLib::DepositRateHelper>(depositQuotes[i].second, depositQuotes[i].first, fixingDays, calendar,
				businessDayConvention, true, depoDayCounter)
		);
	}

	QuantLib::DayCounter fraDayCounter = QuantLib::Actual360();
	for (size_t i = 0; i < fraQuotes.size(); i++) {
		depoFraSwapInstruments.push_back(
			boost::make_shared<QuantLib::FraRateHelper>(fraQuotes[i].second, QuantLib::months(fraQuotes[i].first.first),
				QuantLib::months(fraQuotes[i].first.first + fraQuotes[i].first.second), fixingDays, calendar, 
				businessDayConvention, true, fraDayCounter
				)
		);
	}

	QuantLib::Frequency fixedFrequency = QuantLib::Annual;
	QuantLib::BusinessDayConvention fixedConvention = QuantLib::ModifiedFollowing;
	QuantLib::DayCounter fixedDayCount = QuantLib::Thirty360();
	boost::shared_ptr<QuantLib::IborIndex> iborIndex = boost::make_shared<QuantLib::Euribor6M>();
	QuantLib::Period fwdStart(0, QuantLib::Days);
	for (size_t i = 0; i < swapQuotes.size(); i++) {
		depoFraSwapInstruments.push_back(
			boost::make_shared<QuantLib::SwapRateHelper>(swapQuotes[i].second, swapQuotes[i].first, calendar,
				fixedFrequency, fixedConvention, fixedDayCount, iborIndex, QuantLib::Handle<QuantLib::Quote>(),
				fwdStart)
		);
	}

	QuantLib::DayCounter termStructureDayCounter = QuantLib::Actual360();
	QuantLib::Real tolerence = 1.0e-15;
	boost::shared_ptr<QuantLib::YieldTermStructure> depoFraSwapTermStructure(
		new QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::LogLinear>(settlementDate, depoFraSwapInstruments,
			termStructureDayCounter, tolerence)
	);

	// Check fair swap rate. Forward rate or zero rate is relatively easy to check.
	size_t sz = 5;
	QuantLib::RelinkableHandle<QuantLib::YieldTermStructure> ycHanle(depoFraSwapTermStructure);
	boost::shared_ptr<QuantLib::IborIndex> euriborIndex = boost::make_shared<QuantLib::Euribor6M>(ycHanle);
	QuantLib::VanillaSwap tmpSwap = QuantLib::MakeVanillaSwap(swapQuotes[sz].first, euriborIndex);
	std::cout << "Fair rate of a swap: " << tmpSwap.fairRate() << std::endl;

	// Price a new swap (zero NPV).
	QuantLib::Period fixedTenor(12, QuantLib::Months);
	QuantLib::Period floatTenor(6, QuantLib::Months);

	QuantLib::RelinkableHandle<QuantLib::YieldTermStructure> discountTermStructure(depoFraSwapTermStructure);
	boost::shared_ptr<QuantLib::PricingEngine> swapEngine =
		boost::make_shared<QuantLib::DiscountingSwapEngine>(discountTermStructure);
	QuantLib::VanillaSwap newSwap2evaluate = QuantLib::MakeVanillaSwap(swapQuotes[sz].first, euriborIndex,
		swapQuotes[sz].second, 0 * QuantLib::Days).withNominal(1.0e8)
		.withFixedLegTenor(fixedTenor)
		.withFloatingLegTenor(floatTenor).withType(QuantLib::VanillaSwap::Payer);

	newSwap2evaluate.setPricingEngine(swapEngine);
	std::cout << "NPV of vanilla swap: " << newSwap2evaluate.NPV() << std::endl;

	std::cout << "Computation time: " << timer.elapsed() << std::endl;
	return 0;
}



//int main() {
//	boost::timer timer;
//	QuantLib::Date today(5, QuantLib::November, 2019);
//	QuantLib::DayCounter day_counter = QuantLib::ActualActual();
//	QuantLib::Calendar calendar = QuantLib::TARGET();
//	//std::cout << calendar.isBusinessDay(today - 2) << std::endl;
//	QuantLib::Date settlement_date = today + 2;
//	//QuantLib::Date settlement_date = calendar.advance(today, QuantLib::Period(2, QuantLib::Days));
//	QuantLib::Natural settlement_days = settlement_date - today;
//	QuantLib::Settings::instance().evaluationDate() = today;
//	
//	QuantLib::RelinkableHandle<QuantLib::YieldTermStructure> discount_curve;
//	QuantLib::RelinkableHandle<QuantLib::YieldTermStructure> projection_curve;
//
//	boost::shared_ptr<QuantLib::Eonia> eonian_index = boost::make_shared<QuantLib::Eonia>();   // Remember to try keyword of new!!!
//	boost::shared_ptr<QuantLib::Euribor6M> euribor_index = boost::make_shared<QuantLib::Euribor6M>(projection_curve);
//	
//	std::vector<boost::shared_ptr<QuantLib::RateHelper>> rate_helpers;
//	//std::cout << eonian_index->dayCounter() << std::endl;
//	rate_helpers.push_back(boost::make_shared<QuantLib::DepositRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(-0.0036)), 
//		QuantLib::Period(1, QuantLib::Days), eonian_index->fixingDays(), eonian_index->fixingCalendar(), eonian_index->businessDayConvention(), eonian_index->endOfMonth(), 
//		eonian_index->dayCounter()));
//
//	std::map<QuantLib::Period, QuantLib::Real> eonia_swap_data;
//	eonia_swap_data.insert(std::pair<QuantLib::Period, QuantLib::Real>(QuantLib::Period(6, QuantLib::Months), -0.00353));
//	eonia_swap_data.insert(std::pair<QuantLib::Period, QuantLib::Real>(QuantLib::Period(1, QuantLib::Years), -0.00331));
//	eonia_swap_data.insert(std::pair<QuantLib::Period, QuantLib::Real>(QuantLib::Period(2, QuantLib::Years), -0.00248));
//	eonia_swap_data.insert(std::pair<QuantLib::Period, QuantLib::Real>(QuantLib::Period(3, QuantLib::Years), -0.00138));
//	eonia_swap_data.insert(std::pair<QuantLib::Period, QuantLib::Real>(QuantLib::Period(4, QuantLib::Years), -0.0001245));
//	eonia_swap_data.insert(std::pair<QuantLib::Period, QuantLib::Real>(QuantLib::Period(5, QuantLib::Years), 0.0011945));
//	eonia_swap_data.insert(std::pair<QuantLib::Period, QuantLib::Real>(QuantLib::Period(7, QuantLib::Years), 0.00387));
//	eonia_swap_data.insert(std::pair<QuantLib::Period, QuantLib::Real>(QuantLib::Period(10, QuantLib::Years), 0.007634));	
//	//std::cout << eonia_swap_data.find(QuantLib::Period(6, QuantLib::Months))->first << " and " << 
//	//	eonia_swap_data.find(QuantLib::Period(6, QuantLib::Months))->second << std::endl;
//	
//	std::for_each(eonia_swap_data.begin(), eonia_swap_data.end(),
//		[settlement_days, &rate_helpers, eonian_index] (std::pair<QuantLib::Period, QuantLib::Real> data) {
//			rate_helpers.push_back(boost::make_shared<QuantLib::OISRateHelper>(settlement_days, data.first, 
//				QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(data.second)), eonian_index));
//		});
//	
//	boost::shared_ptr<QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::LogLinear>> eonian_curve =
//		boost::make_shared<QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::LogLinear>>(0, eonian_index->fixingCalendar(), rate_helpers, eonian_index->dayCounter());
//	//boost::shared_ptr<QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::LogLinear>> eonian_curve(
//	//	new QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::LogLinear>(0, eonian_index->fixingCalendar(), rate_helpers, eonian_index->dayCounter()));
//	eonian_curve->enableExtrapolation(true);
//	discount_curve.linkTo(eonian_curve);
//	//std::cout << rate_helpers.capacity() << std::endl;
//	
//	rate_helpers.clear();    // clear rate_helpers containers???
//	//std::cout << rate_helpers.capacity() << std::endl;
//
//	// euribor curve
//	// cash part
//	rate_helpers.push_back(boost::make_shared<QuantLib::DepositRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(-0.00273)), 
//		QuantLib::Period(6, QuantLib::Months), settlement_days, calendar, euribor_index->businessDayConvention(), 
//		euribor_index->endOfMonth(), euribor_index->dayCounter()));
//
//	// fra part
//	rate_helpers.push_back(boost::make_shared<QuantLib::FraRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(-0.00194)), 
//		QuantLib::Period(6, QuantLib::Months), euribor_index));
//	
//	// swap part
//	rate_helpers.push_back(boost::make_shared<QuantLib::SwapRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(-0.00119)), 
//		QuantLib::Period(2, QuantLib::Years), calendar, QuantLib::Annual, QuantLib::ModifiedFollowing, QuantLib::Actual360(), euribor_index, 
//		QuantLib::Handle<QuantLib::Quote>(), QuantLib::Period(0, QuantLib::Days), discount_curve));
//
//	rate_helpers.push_back(boost::make_shared<QuantLib::SwapRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(0.00019)),
//		QuantLib::Period(3, QuantLib::Years), calendar, QuantLib::Annual, QuantLib::ModifiedFollowing, QuantLib::Actual360(), euribor_index,
//		QuantLib::Handle<QuantLib::Quote>(), QuantLib::Period(0, QuantLib::Days), discount_curve));
//
//	rate_helpers.push_back(boost::make_shared<QuantLib::SwapRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(0.00167)),
//		QuantLib::Period(4, QuantLib::Years), calendar, QuantLib::Annual, QuantLib::ModifiedFollowing, QuantLib::Actual360(), euribor_index,
//		QuantLib::Handle<QuantLib::Quote>(), QuantLib::Period(0, QuantLib::Days), discount_curve));
//
//	rate_helpers.push_back(boost::make_shared<QuantLib::SwapRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(0.00317)),
//		QuantLib::Period(5, QuantLib::Years), calendar, QuantLib::Annual, QuantLib::ModifiedFollowing, QuantLib::Actual360(), euribor_index,
//		QuantLib::Handle<QuantLib::Quote>(), QuantLib::Period(0, QuantLib::Days), discount_curve));
//
//	rate_helpers.push_back(boost::make_shared<QuantLib::SwapRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(0.00598)),
//		QuantLib::Period(7, QuantLib::Years), calendar, QuantLib::Annual, QuantLib::ModifiedFollowing, QuantLib::Actual360(), euribor_index,
//		QuantLib::Handle<QuantLib::Quote>(), QuantLib::Period(0, QuantLib::Days), discount_curve));
//
//
//	rate_helpers.push_back(boost::make_shared<QuantLib::SwapRateHelper>(QuantLib::Handle<QuantLib::Quote>(boost::make_shared<QuantLib::SimpleQuote>(0.00966)),
//		QuantLib::Period(10, QuantLib::Years), calendar, QuantLib::Annual, QuantLib::ModifiedFollowing, QuantLib::Actual360(), euribor_index,
//		QuantLib::Handle<QuantLib::Quote>(), QuantLib::Period(0, QuantLib::Days), discount_curve));
//
//	// create euribor curve
//	auto euribor_curve = boost::make_shared<QuantLib::PiecewiseYieldCurve<QuantLib::Discount, QuantLib::LogLinear>>(0, calendar, rate_helpers, euribor_index->dayCounter());
//	euribor_curve->enableExtrapolation();
//	projection_curve.linkTo(euribor_curve);
//
//	// create seasoned vanilla swap
//	QuantLib::Date pastSettlementDate(5, QuantLib::October, 2017);
//
//	QuantLib::Schedule fixed_schedule(pastSettlementDate, pastSettlementDate + QuantLib::Period(5, QuantLib::Years), QuantLib::Period(QuantLib::Annual), 
//		calendar, QuantLib::Unadjusted, QuantLib::Unadjusted, QuantLib::DateGeneration::Backward, false);
//	QuantLib::Schedule floating_schedule(pastSettlementDate, pastSettlementDate + QuantLib::Period(5, QuantLib::Years), QuantLib::Period(QuantLib::Semiannual), 
//		calendar, QuantLib::Unadjusted, QuantLib::Unadjusted, QuantLib::DateGeneration::Backward, false);
//	
//	//std::vector<QuantLib::Date>::const_iterator const_iter;
//	//for (const_iter = floating_schedule.begin(); const_iter < floating_schedule.end(); const_iter++) {
//	//	std::cout << *const_iter << std::endl;
//	//}
//	
//	QuantLib::VanillaSwap swap(QuantLib::VanillaSwap::Payer, 10000000.0, fixed_schedule, 0.0285, day_counter, floating_schedule, euribor_index, 0, day_counter);
//
//	std::cout << "Computing time: " << timer.elapsed() << " seconds" << std::endl; 
//	return 0;
//}



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