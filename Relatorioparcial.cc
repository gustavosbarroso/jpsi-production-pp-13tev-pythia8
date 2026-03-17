#include "Pythia8/Pythia.h"

#include "TROOT.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TF1.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TLegend.h"
#include "TLine.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace Pythia8;

// =====================================================================
// ========================= DADOS EXPERIMENTAIS ========================
// =====================================================================
const int nData = 13;

double ptData[nData] = {
  0.25, 0.75, 1.50, 2.50, 3.50, 4.50,
  6.00, 8.50, 12.50, 17.50, 22.50, 27.50, 35.00
};

double sigmaData[nData] = {
  0.552932, 1.587048, 2.111686, 2.083910, 1.402788,
  0.736975, 0.462506, 0.125494, 0.019369, 0.004190,
  0.001303, 0.000611, 0.000182
};

double errStat[nData] = {
  0.082713, 0.147234, 0.136809, 0.131185, 0.092912,
  0.059509, 0.028899, 0.011422, 0.003804, 0.000352,
  0.000135, 0.000081, 0.000036
};

// =====================================================================
// ============================ BINNING pT ===============================
// =====================================================================
const int nPtBins = 13;

double ptBins[nPtBins + 1] = {
  0., 0.5, 1., 2., 3., 4., 5., 7., 10., 15., 20., 25., 30., 40.
};

// =====================================================================
// ====================== IDENTIFICAÇÃO DE B-HADRONS =====================
// =====================================================================
bool isBHadron(int id) {
  id = std::abs(id);
  if (id == 511 || id == 521 || id == 531 || id == 541) return true;
  if (id / 1000 == 5 || id / 100 == 5) return true;
  return false;
}

bool isFromBHadron(const Event& event, int idx) {
  std::vector<int> stack = {idx};
  std::vector<bool> visited(event.size(), false);

  while (!stack.empty()) {
    int i = stack.back();
    stack.pop_back();
    if (visited[i]) continue;
    visited[i] = true;

    if (isBHadron(event[i].id())) return true;
    for (int m : event[i].motherList())
      if (m > 0) stack.push_back(m);
  }
  return false;
}

// =====================================================================
// ======================= NORMALIZAÇÃO DOUBLE DIFF =====================
// =====================================================================
void NormalizeDoubleDiff(TH1F* h, double lumiEq, double deltaY) {
  for (int i = 1; i <= h->GetNbinsX(); ++i) {
    double c = h->GetBinContent(i);
    double e = h->GetBinError(i);
    double w = h->GetBinWidth(i);
    h->SetBinContent(i, c / (lumiEq * w * deltaY));
    h->SetBinError(i, e / (lumiEq * w * deltaY));
  }
}

// =====================================================================
// ============================ MAIN ====================================
// =====================================================================
int main() {

  // -------------------- TEMPO --------------------
  TStopwatch timer;
  timer.Start();

  // -------------------- PYTHIA SETUP --------------------
  Pythia pythia;
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 13000");
  //Charmonium
  pythia.readString("Charmonium:all = on");
  
  // Tune LHC
  pythia.readString("Tune:pp = 14");
  pythia.readString("Tune:ee = 7");
  
  // Eventos inclusivos
  pythia.readString("SoftQCD:nonDiffractive = on");

  // Charmonium
  pythia.readString("Charmonium:all = on");

  // Parton level
  pythia.readString("PartonLevel:MPI = on");
  pythia.readString("PartonLevel:ISR = on");
  pythia.readString("PartonLevel:FSR = on");

  // Sem corte artificial
  pythia.readString("PhaseSpace:pTHatMin = 0.0");

  // Reprodutibilidade
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 0");
  pythia.init();

  // -------------------- HISTOGRAMAS --------------------
  TH1F* h_prompt    = new TH1F("h_prompt", "", nPtBins, ptBins);
  TH1F* h_nonprompt = new TH1F("h_nonprompt", "", nPtBins, ptBins);
  TH1F* h_total     = new TH1F("h_total", "", nPtBins, ptBins);

  h_prompt->Sumw2();
  h_nonprompt->Sumw2();
  h_total->Sumw2();

  int nEvents = 1e7;
  double yMax = 0.9;
  double deltaY = 2.0 * yMax;

  // -------------------- LOOP DE EVENTOS --------------------
  for (int i = 0; i < nEvents; ++i) {
    if (!pythia.next()) continue;

    for (int j = 0; j < pythia.event.size(); ++j) {
      if (std::abs(pythia.event[j].id()) != 443) continue;
      if (std::abs(pythia.event[j].y()) > yMax) continue;

      double pt = pythia.event[j].pT();
      if (isFromBHadron(pythia.event, j))
        h_nonprompt->Fill(pt);
      else
        h_prompt->Fill(pt);
    }
  }

  // -------------------- NORMALIZAÇÃO --------------------
  double sigmaGen = pythia.info.sigmaGen() * 1000.0; // mb → μb
  int nAccepted   = pythia.info.nAccepted();

  double lumiEq = nAccepted / sigmaGen;

  NormalizeDoubleDiff(h_prompt, lumiEq, deltaY);
  NormalizeDoubleDiff(h_nonprompt, lumiEq, deltaY);

  h_total->Add(h_prompt);
  h_total->Add(h_nonprompt);

  // -------------------- DADOS --------------------
  TH1F* h_data = new TH1F("h_data", "", nPtBins, ptBins);
  h_data->Sumw2();

  for (int i = 0; i < nData; ++i) {
    int bin = h_data->FindBin(ptData[i]);
    h_data->SetBinContent(bin, sigmaData[i]);
    h_data->SetBinError(bin, errStat[i]);
  }

  // -------------------- RAZÃO --------------------
  TH1F* h_ratio = (TH1F*) h_data->Clone("h_ratio");
  h_ratio->Divide(h_total);

  // ===== DEFINIÇÃO DE ESTILO DA RAZÃO (ANTES DA LEGENDA) =====
  h_ratio->SetMarkerStyle(20);
  h_ratio->SetMarkerColor(kRed+1);
  h_ratio->SetLineColor(kRed+1);

  // -------------------- CANVAS --------------------
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "", 900, 900);

  TPad* pad1 = new TPad("pad1", "", 0.0, 0.35, 1.0, 1.0);
  pad1->SetBottomMargin(0.02);
  pad1->SetLeftMargin(0.18);
  pad1->SetRightMargin(0.04);
  pad1->SetLogy();
  pad1->Draw();

  TPad* pad2 = new TPad("pad2", "", 0.0, 0.0, 1.0, 0.35);
  pad2->SetTopMargin(0.02);
  pad2->SetBottomMargin(0.38);
  pad2->SetLeftMargin(0.16);
  pad2->SetRightMargin(0.04);
  pad2->Draw();

  // -------------------- PAD SUPERIOR --------------------
  pad1->cd();
  h_total->GetYaxis()->SetTitle("d^{2}#sigma / (dp_{T} dy) [#mub/(GeV/c)]");
  h_total->GetYaxis()->SetTitleSize(0.065);
  h_total->GetYaxis()->SetLabelSize(0.05);
  h_total->GetYaxis()->SetTitleOffset(1.1);
  h_total->GetXaxis()->SetLabelSize(0.0);
  h_total->Draw("E1");


  TGraphErrors* gData = new TGraphErrors(nData);
  for (int i = 0; i < nData; ++i) {
    double ex = 0.5 * (ptBins[i+1] - ptBins[i]);
    gData->SetPoint(i, ptData[i], sigmaData[i]);
    gData->SetPointError(i, ex, errStat[i]);
  }
  gData->SetMarkerStyle(20);
  gData->Draw("P SAME");

  TLegend* leg = new TLegend(0.55, 0.60, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->AddEntry(h_total, "PYTHIA8", "l");
  leg->AddEntry(gData, "Dados experimentais", "p");
  leg->AddEntry(h_ratio, "Dados / PYTHIA8", "p");
  leg->Draw();

  // -------------------- PAD INFERIOR --------------------
  pad2->cd();
  h_ratio->GetYaxis()->SetTitle("Dados / PYTHIA8");
  h_ratio->GetYaxis()->SetTitleSize(0.10);
  h_ratio->GetYaxis()->SetLabelSize(0.085);
  h_ratio->SetMinimum(0.0);
  h_ratio->SetMaximum(3.0);

  h_ratio->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  h_ratio->GetXaxis()->SetTitleSize(0.12);
  h_ratio->GetXaxis()->SetLabelSize(0.10);
  h_ratio->Draw("E1");

  TLine* line = new TLine(0., 1., 40., 1.);
  line->SetLineStyle(2);
  line->Draw();

  // -------------------- SAÍDAS --------------------
  c->SaveAs("jpsi_alice_style_ratio.png");

  TFile *out = new TFile("jpsi_output.root", "RECREATE");
  h_prompt->Write();
  h_nonprompt->Write();
  h_total->Write();
  h_data->Write();
  h_ratio->Write();
  out->Close();

  // -------------------- TEMPO --------------------
  timer.Stop();
  std::cout << "\n===== TEMPO DE SIMULACAO =====" << std::endl;
  timer.Print();

  return 0;
}
