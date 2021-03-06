#ifndef __GZIP_WRITER_HPP__
#define __GZIP_WRITER_HPP__

#include <memory>
#include <mutex>

#include "spdlog/spdlog.h"

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include "ReadExperiment.hpp"
#include "SalmonOpts.hpp"
#include "SalmonSpinLock.hpp"
#include "AlevinOpts.hpp"

class GZipWriter {
public:
  GZipWriter(const boost::filesystem::path path,
             std::shared_ptr<spdlog::logger> logger);

  ~GZipWriter();

  void close_all_streams();

  template <typename ExpT>
  bool writeEquivCounts(const SalmonOpts& opts, ExpT& experiment);

  template <typename ExpT, typename ProtocolT>
  bool writeEquivCounts(const AlevinOpts<ProtocolT>& aopts,
                        ExpT& experiment);

  template <typename ExpT>
  bool writeBFH(boost::filesystem::path& outDir,
                ExpT& experiment, size_t umiLength,
                std::vector<std::string>& bcSeqVec);

  template <typename ExpT>
  bool writeMeta(const SalmonOpts& opts, const ExpT& experiment);

  template <typename ExpT>
  bool writeEmptyMeta(const SalmonOpts& opts, const ExpT& experiment,
                      std::vector<std::string>& errors);

  template <typename ExpT>
  bool writeAbundances(const SalmonOpts& sopt, ExpT& readExp);

  bool writeAbundances(std::vector<double>& alphas,
                       std::vector<Transcript>& transcripts);

  bool writeAbundances(bool inDebugMode,
                       std::string bcName,
                       std::vector<double>& alphas);

  template <typename ExpT>
  bool writeEmptyAbundances(const SalmonOpts& sopt, ExpT& readExp);

  template <typename T>
  bool writeBootstrap(const std::vector<T>& abund, bool quiet = false);

  bool writeCellEQVec(size_t barcode, const std::vector<uint32_t>& offsets, const std::vector<uint32_t>& counts, bool quiet = true);

  bool setSamplingPath(const SalmonOpts& sopt);

private:
  boost::filesystem::path path_;
  boost::filesystem::path bsPath_;
  std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<boost::iostreams::filtering_ostream> bsStream_{nullptr};
  std::unique_ptr<boost::iostreams::filtering_ostream> countMatrixStream_{nullptr};
  std::unique_ptr<std::ofstream> bcNameStream_{nullptr};
  std::unique_ptr<boost::iostreams::filtering_ostream> cellEQStream_{nullptr};
// only one writer thread at a time
#if defined __APPLE__
  spin_lock writeMutex_;
#else
  std::mutex writeMutex_;
#endif
  std::atomic<uint32_t> numBootstrapsWritten_{0};
};

#endif //__GZIP_WRITER_HPP__
