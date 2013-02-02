#include "cm_match.h"
#include "cm_util.h"
#include "cm_account.h"
#include "cm_context.h"
#include <postgresql/libpq-fe.h>
#include <uuid/uuid.h>
#include <sstream>

void cmdev_list_apps(evhtp_request_t *req, void *arg) {
    const char* err_auth = "err_auth";
    const char* err_db = "err_db";
    const char* err_db_select = "err_db_select";
    
    PGconn *cmatchConn = cm_get_context()->cmatchdb;
    if (PQstatus(cmatchConn) != CONNECTION_OK) {
        return cm_send_err(err_db, req);
    }
    
    //check auth
    cm_session session;
    int err = cm_find_session(req, session);
    if (err) {
        return cm_send_err(err_auth, req);
    }
    
    //qurey db
    std::stringstream sssql;
    sssql << "SELECT id, name FROM app WHERE developer_id=" << session.userid;
    PGresult *res = PQexec(cmatchConn, sssql.str().c_str());
    Autofree af_res_cl(res, (Freefunc)PQclear);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        return cm_send_err(err_db_select, req);
    }
    
    int rownums = PQntuples(res);
    if (rownums == 0) {
        return cm_send_ok(req);
    }
    
    std::stringstream ssout;
    ssout << "{\"error\":0,\"apps\":{";
    
    bool first = true;
    for (int i = 0; i < rownums; ++i) {
        const char* sid = PQgetvalue(res, i, 0);
        const char* sname = PQgetvalue(res, i, 1);
        if (sid && sname) {
            if (first) {
                first = false;
            } else {
                ssout << ",";
            }
            ssout << "\"" << sid << "\"" << ":\"" << sname << "\"";
        }
    }
    ssout << "}}";
    
    //send to client
    evbuffer_add_printf(req->buffer_out, "%s", ssout.str().c_str());
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void cmdev_add_app(evhtp_request_t *req, void *arg) {
    const char* err_param = "err_param";
    const char* err_auth = "err_auth";
    const char* err_db = "err_db";
    const char* err_db_insert = "err_db_insert";
    const char* err_limit = "err_limit";
    const uint32_t APP_NUM_LIMIT = 5;

    PGconn *cmatchConn = cm_get_context()->cmatchdb;
    if (PQstatus(cmatchConn) != CONNECTION_OK) {
        return cm_send_err(err_db, req);
    }
    
    //parse param
    int err = 0;
    const char *name = kvs_find_string(&err, req->uri->query, "appname");
    if (err) {
        return cm_send_err(err_param, req);
    }
    std::string strname;
    url_decode(strname, name);
    name = strname.c_str();
    
    //check auth
    cm_session session;
    err = cm_find_session(req, session);
    if (err) {
        return cm_send_err(err_auth, req);
    }
    
    //secret key
    uuid_t uuid;
    uuid_generate(uuid);
    char *secret = base64_cf((const char*)uuid, sizeof(uuid));
    Autofree af_token(secret);
    
    //count limit
    std::stringstream ss_sql;
    ss_sql << "SELECT COUNT(0) FROM app WHERE developer_id=" << session.userid << ";";
    PGresult *res = PQexec(cmatchConn, ss_sql.str().c_str());
    Autofree af_res_cl(res, (Freefunc)PQclear);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        return cm_send_err(err_db, req);
    }
    const char* sappnum = PQgetvalue(res, 0, 0);
    if (sappnum == NULL) {
        return cm_send_err(err_db, req);
    }
    err = 0;
    uint32_t appnum = s2int32(sappnum, &err);
    if (err){
        return cm_send_err(err_db, req);
    }
    if (appnum >= APP_NUM_LIMIT) {
        return cm_send_err(err_limit, req);
    }
    
    //insert into db
    std::stringstream ss_developer_id;
    ss_developer_id << session.userid;
    const char *vars[3] = {
        name,
        secret,
        ss_developer_id.str().c_str(),
    };
    res = PQexecParams(cmatchConn,
                       "INSERT INTO app (name, secret, developer_id) VALUES($1, $2, $3) RETURNING id;",
                       3, NULL, vars, NULL, NULL, 0);
    Autofree af_res_insert(res, (Freefunc)PQclear);
    if (PQresultStatus(res) != PGRES_TUPLES_OK){
        return cm_send_err(err_db_insert, req);
    }

    //get app id
    const char* appid = PQgetvalue(res, 0, 0);
    if (appid == NULL) {
        return cm_send_err(err_db, req);
    }
    
    //send to client
    evbuffer_add_printf(req->buffer_out, "{\"error\":0, \"id\":%s}", appid);
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void cmdev_edit_app(evhtp_request_t *req, void *arg) {
    const char* err_param = "err_param";
    const char* err_db = "err_db";
    const char* err_auth = "err_auth";
    const char* err_appnotexist = "err_appnotexist";
    
    PGconn *cmatchConn = cm_get_context()->cmatchdb;
    if (PQstatus(cmatchConn) != CONNECTION_OK) {
        return cm_send_err(err_db, req);
    }
    
    //parse param
    int err = 0;
    const char *appid = kvs_find_string(&err, req->uri->query, "appid");
    const char *appname = kvs_find_string(&err, req->uri->query, "appname");
    if (err) {
        return cm_send_err(err_param, req);
    }
    std::string sappname;
    url_decode(sappname, appname);
    appname = sappname.c_str();
    
    //check auth
    cm_session session;
    err = cm_find_session(req, session);
    if (err) {
        return cm_send_err(err_auth, req);
    }
    
    //update db
    const int VARNUM = 3;
    const char *vars[VARNUM] = {
        appname,
        appid,
        session.userid.c_str(),
    };
    PGresult *res = PQexecParams(cmatchConn,
                       "UPDATE app SET name=$1 WHERE id=$2 AND developer_id=$3",
                       VARNUM, NULL, vars, NULL, NULL, 0);
    Autofree af_res_insert(res, (Freefunc)PQclear);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        return cm_send_err(err_db, req);
    }
    
    const char *affected = PQcmdTuples(res);
    if (!affected || *affected == '0') {
        return cm_send_err(err_appnotexist, req);
    }
    
    //send to client
    evbuffer_add_printf(req->buffer_out, "{\"error\":0}");
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void cmdev_get_app_secret(evhtp_request_t *req, void *arg) {
    const char* err_param = "err_param";
    const char* err_db = "err_db";
    const char* err_auth = "err_auth";
    const char* err_notexist = "err_notexist";
    
    PGconn *cmatchConn = cm_get_context()->cmatchdb;
    if (PQstatus(cmatchConn) != CONNECTION_OK) {
        return cm_send_err(err_db, req);
    }
    
    //parse param
    int err = 0;
    uint64_t appid = kvs_find_uint64(&err, req->uri->query, "appid");
    if (err) {
        return cm_send_err(err_param, req);
    }
    
    //check auth
    cm_session session;
    err = cm_find_session(req, session);
    if (err) {
        return cm_send_err(err_auth, req);
    }
    
    //select from db
    std::stringstream ss_sql;
    ss_sql << "SELECT secret FROM app WHERE developer_id=" << session.userid << " AND id=" << appid << ";";
    PGresult *res = PQexec(cmatchConn, ss_sql.str().c_str());
    Autofree af_res_cl(res, (Freefunc)PQclear);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        return cm_send_err(err_db, req);
    }
    
    int rows = PQntuples(res);
    if (rows != 1) {
        return cm_send_err(err_notexist, req);
    }
    const char* secret = PQgetvalue(res, 0, 0);
    if (secret == NULL) {
        return cm_send_err(err_db, req);
    }
    
    //send to client
    evbuffer_add_printf(req->buffer_out, "{\"error\":0, \"secret\":%s}", secret);
    evhtp_send_reply(req, EVHTP_RES_OK);
}