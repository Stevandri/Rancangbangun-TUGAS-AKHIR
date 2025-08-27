--
-- PostgreSQL database dump
--

-- Dumped from database version 16.9
-- Dumped by pg_dump version 16.9

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: aktivitas; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.aktivitas (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    tipe text NOT NULL,
    user_id uuid,
    titipan_id uuid,
    loker_id uuid,
    waktu timestamp with time zone DEFAULT now(),
    lokasi_lat double precision,
    lokasi_long double precision,
    status_berhasil boolean,
    CONSTRAINT aktivitas_tipe_check CHECK ((tipe = ANY (ARRAY['titip'::text, 'ambil'::text])))
);


ALTER TABLE public.aktivitas OWNER TO postgres;

--
-- Name: lockers; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.lockers (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    nama_loker text NOT NULL,
    koordinat_lat double precision,
    koordinat_long double precision,
    status text DEFAULT 'kosong'::text,
    terkunci boolean DEFAULT true,
    CONSTRAINT lockers_status_check CHECK ((status = ANY (ARRAY['kosong'::text, 'terisi'::text])))
);


ALTER TABLE public.lockers OWNER TO postgres;

--
-- Name: notifikasi; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.notifikasi (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    user_id uuid,
    judul text,
    pesan text,
    dibaca boolean DEFAULT false,
    waktu timestamp with time zone DEFAULT now()
);


ALTER TABLE public.notifikasi OWNER TO postgres;

--
-- Name: sensor_status; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sensor_status (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    loker_id uuid,
    berat double precision,
    ultrasonik_cm double precision,
    pintu_terbuka boolean,
    terakhir_update timestamp with time zone DEFAULT now()
);


ALTER TABLE public.sensor_status OWNER TO postgres;

--
-- Name: titipan; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.titipan (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    pengirim_id uuid,
    penerima_id uuid,
    loker_id uuid,
    deskripsi text,
    foto_url text,
    berat double precision,
    waktu_titip timestamp with time zone DEFAULT now(),
    waktu_diambil timestamp with time zone,
    kode_ambil text,
    status text DEFAULT 'dititipkan'::text,
    CONSTRAINT titipan_status_check CHECK ((status = ANY (ARRAY['dititipkan'::text, 'diambil'::text, 'kedaluwarsa'::text])))
);


ALTER TABLE public.titipan OWNER TO postgres;

--
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.users (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    nama text NOT NULL,
    email text NOT NULL,
    password_hash text NOT NULL,
    role text NOT NULL,
    status boolean DEFAULT true,
    created_at timestamp with time zone DEFAULT now(),
    CONSTRAINT users_role_check CHECK ((role = ANY (ARRAY['user'::text, 'admin'::text])))
);


ALTER TABLE public.users OWNER TO postgres;

--
-- Name: wifi_config; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.wifi_config (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    ssid text NOT NULL,
    password text
);


ALTER TABLE public.wifi_config OWNER TO postgres;

--
-- Data for Name: aktivitas; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.aktivitas (id, tipe, user_id, titipan_id, loker_id, waktu, lokasi_lat, lokasi_long, status_berhasil) FROM stdin;
\.


--
-- Data for Name: lockers; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.lockers (id, nama_loker, koordinat_lat, koordinat_long, status, terkunci) FROM stdin;
\.


--
-- Data for Name: notifikasi; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.notifikasi (id, user_id, judul, pesan, dibaca, waktu) FROM stdin;
\.


--
-- Data for Name: sensor_status; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.sensor_status (id, loker_id, berat, ultrasonik_cm, pintu_terbuka, terakhir_update) FROM stdin;
\.


--
-- Data for Name: titipan; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.titipan (id, pengirim_id, penerima_id, loker_id, deskripsi, foto_url, berat, waktu_titip, waktu_diambil, kode_ambil, status) FROM stdin;
\.


--
-- Data for Name: users; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.users (id, nama, email, password_hash, role, status, created_at) FROM stdin;
\.


--
-- Data for Name: wifi_config; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.wifi_config (id, ssid, password) FROM stdin;
\.


--
-- Name: aktivitas aktivitas_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.aktivitas
    ADD CONSTRAINT aktivitas_pkey PRIMARY KEY (id);


--
-- Name: lockers lockers_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.lockers
    ADD CONSTRAINT lockers_pkey PRIMARY KEY (id);


--
-- Name: notifikasi notifikasi_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.notifikasi
    ADD CONSTRAINT notifikasi_pkey PRIMARY KEY (id);


--
-- Name: sensor_status sensor_status_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_status
    ADD CONSTRAINT sensor_status_pkey PRIMARY KEY (id);


--
-- Name: titipan titipan_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.titipan
    ADD CONSTRAINT titipan_pkey PRIMARY KEY (id);


--
-- Name: users users_email_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_email_key UNIQUE (email);


--
-- Name: users users_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_pkey PRIMARY KEY (id);


--
-- Name: wifi_config wifi_config_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.wifi_config
    ADD CONSTRAINT wifi_config_pkey PRIMARY KEY (id);


--
-- Name: aktivitas aktivitas_loker_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.aktivitas
    ADD CONSTRAINT aktivitas_loker_id_fkey FOREIGN KEY (loker_id) REFERENCES public.lockers(id) ON DELETE CASCADE;


--
-- Name: aktivitas aktivitas_titipan_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.aktivitas
    ADD CONSTRAINT aktivitas_titipan_id_fkey FOREIGN KEY (titipan_id) REFERENCES public.titipan(id) ON DELETE CASCADE;


--
-- Name: aktivitas aktivitas_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.aktivitas
    ADD CONSTRAINT aktivitas_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id) ON DELETE SET NULL;


--
-- Name: notifikasi notifikasi_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.notifikasi
    ADD CONSTRAINT notifikasi_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id) ON DELETE CASCADE;


--
-- Name: sensor_status sensor_status_loker_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_status
    ADD CONSTRAINT sensor_status_loker_id_fkey FOREIGN KEY (loker_id) REFERENCES public.lockers(id) ON DELETE CASCADE;


--
-- Name: titipan titipan_loker_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.titipan
    ADD CONSTRAINT titipan_loker_id_fkey FOREIGN KEY (loker_id) REFERENCES public.lockers(id) ON DELETE CASCADE;


--
-- Name: titipan titipan_penerima_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.titipan
    ADD CONSTRAINT titipan_penerima_id_fkey FOREIGN KEY (penerima_id) REFERENCES public.users(id) ON DELETE SET NULL;


--
-- Name: titipan titipan_pengirim_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.titipan
    ADD CONSTRAINT titipan_pengirim_id_fkey FOREIGN KEY (pengirim_id) REFERENCES public.users(id) ON DELETE SET NULL;


--
-- PostgreSQL database dump complete
--

