﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReadFortrade1
{
    public class InstrumentValuePair
    {
        public InstrumentValuePair(long pStamp, double pSell)
        {
            Stamp = pStamp;
            SellValue = pSell;
        }
        public long Stamp;
        public double SellValue;
    }
    //number of seconds in an interval
    public class TimeValues
    {
        public static int MinuteToSecond = 60; // 60 seconds in 1 minute
        public static int HourToSecond = 60 * MinuteToSecond;
        public static int DayToSecond = 24 * HourToSecond;
        public static int DayToMinute = 24 * 60;
    }
    public class ValueStatistics
    {
        /*
         * DaysInPast = 0 means current day
         */
        public static void GetInstrumentDailyInversionCount(string InstrumentName, int DaysInPast, int DaysLong, double ExpectedProfitFlat, double ExpectedProfitPCTSpread)
        {
            //get all values 
            long StartStamp = DBHandler.GetUnixStampStartDay(0, 0, -DaysInPast);
            long EndStamp = DBHandler.GetUnixStampStartDay(0, 0, -DaysInPast + DaysLong);
            List<InstrumentValuePair> values = Globals.Persistency.GetInstrumentValues(InstrumentName, StartStamp, EndStamp);
            double SpreadAvg = Globals.Persistency.GetInstrumentSpread(InstrumentName, StartStamp, EndStamp);
            //if sell value drops below spread + profit, we call it an inversion point
            //check each inversion point. We presume we buy/sell at first value
            int TimesWeCouldSell = 0;
            int TimesWeCouldBuy = 0;
            double PrevBuyAtValue = 0;
            double PrevSellAtValue = 0;
            double ExpectedProfitFlatSpread = SpreadAvg * ExpectedProfitPCTSpread / 100;
            foreach (InstrumentValuePair itr in values)
            {
                double CurSellValue = itr.SellValue;
                double CurBuyValue = itr.SellValue + SpreadAvg;
                if (PrevSellAtValue == 0)
                    PrevSellAtValue = CurSellValue - SpreadAvg;
                if (PrevBuyAtValue == 0)
                    PrevBuyAtValue = CurBuyValue + SpreadAvg;

                if (CurSellValue + ExpectedProfitFlat < PrevSellAtValue || CurSellValue + ExpectedProfitFlatSpread < PrevSellAtValue)
                {
                    PrevSellAtValue = CurSellValue - SpreadAvg;
                    TimesWeCouldSell++;
                }
                if (CurBuyValue - ExpectedProfitFlat > PrevBuyAtValue || CurBuyValue - ExpectedProfitFlatSpread > PrevBuyAtValue)
                {
                    PrevBuyAtValue = CurBuyValue + SpreadAvg;
                    TimesWeCouldBuy++;
                }
            }
            Globals.Logger.Log("Could flip " + InstrumentName + " sell = " + TimesWeCouldSell.ToString() + " buy = " + TimesWeCouldBuy.ToString() + " times ");
        }

        public static void CalcInversionEachInstrument(int SinceDaysInPast,int NumberOfDays, double ExpectedProfitFlat, double ExpectedProfitPCTSpread)
        {
            List<string> InstrumentsWithValues = Globals.Persistency.GetAllInstrumentNames();
            foreach (string itr in InstrumentsWithValues)
                GetInstrumentDailyInversionCount(itr, SinceDaysInPast, NumberOfDays, ExpectedProfitFlat, ExpectedProfitPCTSpread);
        }

        public static void TransactionsDoneInPeriod(string InstrumentName, int PeriodInMinutes, int DurationMinutes)
        {
            int NumberOfPeriods = DurationMinutes / PeriodInMinutes;
            long Now = DBHandler.GetUnixStamp();
            string ToPrint = "";
            for (int PeriodIndex = 0; PeriodIndex < NumberOfPeriods; PeriodIndex++)
            {
                long StartStamp = Now - (PeriodIndex-0) * PeriodInMinutes * TimeValues.MinuteToSecond;
                long EndStamp = Now - (PeriodIndex-1) * PeriodInMinutes * TimeValues.MinuteToSecond;
                List<InstrumentValuePair> values = Globals.Persistency.GetInstrumentValues(InstrumentName, StartStamp, EndStamp);
                int TransactionsDoneInPeriod = values.Count;
                ToPrint += TransactionsDoneInPeriod.ToString() + ",";
            }
            Globals.Logger.Log("Transaction daily for " + InstrumentName + " : " + ToPrint);
        }

        public static void CalcTransactionsEachInstrument()
        {
            List<string> InstrumentsWithValues = Globals.Persistency.GetAllInstrumentNames();
            foreach (string itr in InstrumentsWithValues)
                TransactionsDoneInPeriod(itr, TimeValues.DayToMinute, TimeValues.DayToMinute * 10);
        }

        public static void GetTopXLargestTransactions(string InstrumentName, int PeriodInMinutes)
        {
            long Now = DBHandler.GetUnixStamp();
            long StartStamp = Now - PeriodInMinutes * TimeValues.MinuteToSecond;
            long EndStamp = Now;
            List<InstrumentValuePair> values = Globals.Persistency.GetInstrumentValues(InstrumentName, StartStamp, EndStamp);
            //convert the values to differences
            double PrevValue = 0;
            foreach(var itr in values)
            {
                if(PrevValue == 0)
                {
                    PrevValue = itr.SellValue;
                    itr.SellValue = 0;
                    continue;
                }
                double NewValue = Math.Abs(itr.SellValue - PrevValue);
                PrevValue = itr.SellValue;
                itr.SellValue = NewValue;
            }

            //sort list by transaction value
            values.Sort(delegate (InstrumentValuePair x, InstrumentValuePair y)
            {
                if (x.SellValue < y.SellValue) return -1;
                if (x.SellValue > y.SellValue) return 1;
                return 0;
            });
            int PrintedValueCount = 0;
            string ToPrint = "";
            foreach(var itr in values)
            {
                ToPrint += itr.SellValue.ToString() + ",";
                PrintedValueCount++;
                if (PrintedValueCount > 10)
                    break;
            }
            Globals.Logger.Log("Top transactoins daily for " + InstrumentName + " : " + ToPrint);
        }

        public static void GetTopXLastestTransactionsAllInstruments()
        {
            List<string> InstrumentsWithValues = Globals.Persistency.GetAllInstrumentNames();
            foreach (string itr in InstrumentsWithValues)
                GetTopXLargestTransactions(itr, TimeValues.DayToMinute * 10);
        }

        public static void GetChangePCT(string InstrumentName, int PeriodInMinutes, int PeriodShiftMinutes, int NumberOfPeriods = 1)
        {
            long Now = DBHandler.GetUnixStamp();
            string ToPrint = "";
            for (int i = 0; i < NumberOfPeriods; i++)
            {
                long StartStamp1 = Now - i * PeriodShiftMinutes - 1 * PeriodInMinutes * TimeValues.MinuteToSecond;
                long EndStamp1 = Now - i * PeriodShiftMinutes - 0 * PeriodInMinutes * TimeValues.MinuteToSecond;
                double Average1 = Globals.Persistency.GetInstrumentAveragePricePeriod(InstrumentName, StartStamp1, EndStamp1);
                long StartStamp2 = Now - i * PeriodShiftMinutes - 2 * PeriodInMinutes * TimeValues.MinuteToSecond;
                long EndStamp2 = Now - i * PeriodShiftMinutes - 1 * PeriodInMinutes * TimeValues.MinuteToSecond;
                double Average2 = Globals.Persistency.GetInstrumentAveragePricePeriod(InstrumentName, StartStamp2, EndStamp2);
                double PCTChange = Math.Round(( Average1 - Average2 ) * 100 / Average2,2);
                ToPrint += PCTChange.ToString() + ",";
            }
            Globals.Logger.Log("Change PCT for " + InstrumentName + " : " + ToPrint);
        }

        public static void GetChangePCTAllInstruments()
        {
            List<string> InstrumentsWithValues = Globals.Persistency.GetAllInstrumentNames();
            foreach (string itr in InstrumentsWithValues)
                GetChangePCT(itr, TimeValues.DayToMinute, TimeValues.HourToSecond, 10);
        }
        /*        public List<InstrumentValuePair> ExpandToPrecision(List<InstrumentValuePair> db, int PrecisionSeconds)
                {
                    long SecondsInDay = 24 * 1 * 60 * 60;
                    long TimeSlots = SecondsInDay/PrecisionSeconds;
                    return db;
                }*/
    }
}
