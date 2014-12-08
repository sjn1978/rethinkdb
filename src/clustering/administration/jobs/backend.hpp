// Copyright 2010-2014 RethinkDB, all rights reserved.
#ifndef CLUSTERING_ADMINISTRATION_JOBS_BACKEND_HPP_
#define CLUSTERING_ADMINISTRATION_JOBS_BACKEND_HPP_

#include <string>
#include <vector>

#include "errors.hpp"
#include <boost/shared_ptr.hpp>

#include "rdb_protocol/artificial_table/backend.hpp"
#include "clustering/administration/metadata.hpp"
#include "concurrency/watchable.hpp"

class server_name_client_t;

class jobs_artificial_table_backend_t :
    public artificial_table_backend_t
{
public:
    jobs_artificial_table_backend_t(
        mailbox_manager_t *_mailbox_manager,
        boost::shared_ptr<semilattice_readwrite_view_t<
            cluster_semilattice_metadata_t> > _semilattice_view,
        const clone_ptr_t<watchable_t<change_tracking_map_t<
            peer_id_t, cluster_directory_metadata_t> > > &_directory_view,
        watchable_map_t<std::pair<peer_id_t, namespace_id_t>,
            namespace_directory_metadata_t> *_reactor_directory_view,
        server_name_client_t *_name_client,
        admin_identifier_format_t _identifier_format);


    std::string get_primary_key_name();

    bool read_all_rows_as_vector(signal_t *interruptor,
                                 std::vector<ql::datum_t> *rows_out,
                                 std::string *error_out);

    bool read_row(ql::datum_t primary_key,
                  signal_t *interruptor,
                  ql::datum_t *row_out,
                  std::string *error_out);

    bool write_row(ql::datum_t primary_key,
                   bool pkey_was_autogenerated,
                   ql::datum_t *new_value_inout,
                   signal_t *interruptor,
                   std::string *error_out);

private:
    void get_all_job_reports(
            signal_t *interruptor,
            std::map<uuid_u, job_report_t> *job_reports_out);

    mailbox_manager_t *mailbox_manager;

    boost::shared_ptr<semilattice_readwrite_view_t<
        cluster_semilattice_metadata_t> > semilattice_view;

    clone_ptr_t<watchable_t<change_tracking_map_t<peer_id_t,
        cluster_directory_metadata_t> > > directory_view;

    watchable_map_t<std::pair<peer_id_t, namespace_id_t>,
        namespace_directory_metadata_t> *reactor_directory_view;

    server_name_client_t *name_client;

    admin_identifier_format_t identifier_format;
};

#endif /* CLUSTERING_ADMINISTRATION_JOBS_BACKEND_HPP_ */
