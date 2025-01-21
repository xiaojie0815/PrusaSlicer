#ifndef SEQARRANGEJOB_HPP
#define SEQARRANGEJOB_HPP

#include "Job.hpp"

namespace Slic3r {

class Model;


class SeqArrange;

namespace GUI {


class SeqArrangeJob : public Job
{
public:
    explicit SeqArrangeJob(const Model& model, const DynamicPrintConfig& config);
    virtual void process(Ctl &ctl) override;
    virtual void finalize(bool /*canceled*/, std::exception_ptr&) override;

private:
    std::unique_ptr<SeqArrange> m_seq_arrange;
};

} // namespace GUI
} // namespace Slic3r

#endif // ARRANGEJOB2_HPP
