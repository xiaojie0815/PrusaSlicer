#include "SeqArrangeJob.hpp"

#include "libslic3r/ArrangeHelper.hpp"

#include "slic3r/GUI/GLCanvas3D.hpp"
#include "slic3r/GUI/GUI_App.hpp"
#include "slic3r/GUI/GUI_ObjectList.hpp"
#include "slic3r/GUI/I18N.hpp"
#include "slic3r/GUI/Plater.hpp"




namespace Slic3r { namespace GUI {


SeqArrangeJob::SeqArrangeJob(const Model& model, const DynamicPrintConfig& config, bool current_bed_only)
{
    m_seq_arrange.reset(new SeqArrange(model, config, current_bed_only));
}


void SeqArrangeJob::process(Ctl& ctl)
{
    class SeqArrangeJobException : std::exception {};

    try {
        m_seq_arrange->process_seq_arrange([&](int progress) {
                ctl.update_status(progress, _u8L("Arranging for sequential print"));
                if (ctl.was_canceled())
                    throw SeqArrangeJobException();
            }
        );
    } catch (const SeqArrangeJobException&) {
        // The task was canceled. Just make sure that the progress notification disappears.
        ctl.update_status(100, "");
    }
}



void SeqArrangeJob::finalize(bool canceled, std::exception_ptr&)
{
    // If the task was cancelled, the stopping exception was already caught
    // in 'process' function. Let any other exception propagate further.
    if (! canceled) {
        Plater::TakeSnapshot snapshot(wxGetApp().plater(), _u8L("Arrange for sequential print"));
        m_seq_arrange->apply_seq_arrange(wxGetApp().model());
        wxGetApp().plater()->canvas3D()->reload_scene(true, true);
        wxGetApp().obj_list()->update_after_undo_redo();
    }
    m_seq_arrange.reset();
}




} // namespace GUI
} // namespace Slic3r
