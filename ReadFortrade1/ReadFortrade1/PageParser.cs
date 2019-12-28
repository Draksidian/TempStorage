﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReadFortrade1
{
    public class PageParser
    {
        public static void ParseBody(string docBody)
        {
            //search for the favorites section
            //<div class="instrumentsTable" id="instrumentsTable" style="height: 256px; z-index: 2;">
            int StartOfDiv = docBody.IndexOf("<div class=\"instrumentsTable\" id=\"instrumentsTable\"");
            int OpenDivCount = 1;
            int ClosedDivCount = 0;
            string FavoritesSection = "";
            for (int i = StartOfDiv + 1; i < docBody.Length - 4; i++)
            {
                if (docBody[i] == '<' && docBody[i + 1] == 'd' && docBody[i + 2] == 'i' && docBody[i + 3] == 'v')
                    OpenDivCount++;
                if (docBody[i] == '<' && docBody[i + 1] == '/' && docBody[i + 2] == 'd' && docBody[i + 3] == 'i' && docBody[i + 4] == 'v')
                    ClosedDivCount++;
                if (OpenDivCount == ClosedDivCount)
                    FavoritesSection = docBody.Substring(StartOfDiv, i - StartOfDiv);
            }
            //find the closing </div>
            if (FavoritesSection != "")
                ParseFavoritesSection(FavoritesSection);
        }

        public static void ParseFavoritesSection(string FavoritesSection)
        {
            string ToSearch = "CFD on the daily spot price of";
            string[] Sections = FavoritesSection.Split(new[] { ToSearch }, StringSplitOptions.None);
            for (int i = 1; i < Sections.Length; i++)
            {
                //get name
                string Name;
                int NameStart = 0;
                for (; NameStart < Sections[i].Length; NameStart++)
                    if (Sections[i][NameStart] == '.' || Sections[i][NameStart + 1] == '<')
                        break;
                Name = Sections[i].Substring(1, NameStart - 1);

                //get Sell price
                ToSearch = "id=\"SellRate";
                int SellDivStart = Sections[i].IndexOf(ToSearch) + ToSearch.Length;
                for (; SellDivStart < Sections[i].Length; SellDivStart++)
                    if (Sections[i][SellDivStart] == '>')
                        break;
                SellDivStart++;
                int SellDivEnd = Sections[i].IndexOf('<', SellDivStart);
                double SellPrice = 0;
                double.TryParse(Sections[i].Substring(SellDivStart, SellDivEnd - SellDivStart), out SellPrice);

                //sell sentiment
                double SellSentiment = 0;
                double BuySentiment = 0;
                ToSearch = "Currently, ";
                int SellSentimentStart = Sections[i].IndexOf(ToSearch, SellDivStart);
                if (SellSentimentStart >= 0)
                {
                    SellSentimentStart += ToSearch.Length;
                    int SellSentimentEnd = Sections[i].IndexOf('%', SellSentimentStart);
                    double.TryParse(Sections[i].Substring(SellSentimentStart, SellSentimentEnd - SellSentimentStart), out SellSentiment);

                    //buy sentiment
                    ToSearch = "are BUY and ";
                    int BuySentimentStart = Sections[i].IndexOf(ToSearch, SellSentimentEnd);
                    if (BuySentimentStart >= 0)
                    {
                        SellSentimentStart += ToSearch.Length;
                        int BuySentimentEnd = Sections[i].IndexOf('%', BuySentimentStart);
                        double.TryParse(Sections[i].Substring(BuySentimentStart, BuySentimentEnd - BuySentimentStart), out BuySentiment);
                    }
                }

                //get Buy price
                ToSearch = "id=\"BuyRate";
                int BuyDivStart = Sections[i].IndexOf(ToSearch, SellDivEnd) + ToSearch.Length;
                for (; BuyDivStart < Sections[i].Length; BuyDivStart++)
                    if (Sections[i][BuyDivStart] == '>')
                        break;
                BuyDivStart++;
                int BuyDivEnd = Sections[i].IndexOf('<', BuyDivStart);
                double BuyPrice = 0;
                double.TryParse(Sections[i].Substring(BuyDivStart, BuyDivEnd - BuyDivStart), out BuyPrice);

                if (SellPrice != 0 && SellSentiment != 0 && BuySentiment != 0 && BuyPrice != 0)
                    Globals.vHistory.AddRecord(Name, SellPrice, BuyPrice, SellSentiment, BuySentiment);
            }
        }
    }
}
